 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
goto _LL27; _LL27: for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp28=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
int _temp44; _LL30: if(( unsigned int) _temp28 > 16u?*(( int*) _temp28) == Cyc_Absyn_Regparm_att:
0){ _LL45: _temp44=(( struct Cyc_Absyn_Regparm_att_struct*) _temp28)->f1; goto
_LL31;} else{ goto _LL32;} _LL32: if( _temp28 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp28 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp28 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL37;} else{ goto _LL38;} _LL38: if( _temp28 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL39;} else{ goto _LL40;} _LL40: if( _temp28 ==( void*) Cyc_Absyn_Const_att){
goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43; _LL31: goto _LL33; _LL33:
goto _LL35; _LL35: goto _LL37; _LL37: goto _LL39; _LL39: goto _LL41; _LL41:*
_temp11=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp46->tl=* _temp11; _temp46;}); continue; _LL43:
_temp26=({ struct Cyc_List_List* _temp47=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp47->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp47->tl= _temp26; _temp47;}); continue; _LL29:;}
return _temp26;} _LL6: return(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts", sizeof( unsigned char), 22u));
_LL2:;} struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd, int check_var_init){ struct _tagged_arr* v=(*
vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List*
atts= vd->attributes;{ void* _temp48=(* vd->name).f1; struct Cyc_List_List*
_temp56; struct Cyc_List_List* _temp58; _LL50: if(( unsigned int) _temp48 > 1u?*((
int*) _temp48) == Cyc_Absyn_Rel_n: 0){ _LL57: _temp56=(( struct Cyc_Absyn_Rel_n_struct*)
_temp48)->f1; if( _temp56 == 0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Abs_n:
0){ _LL59: _temp58=(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp58
== 0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: goto _LL55;
_LL51: goto _LL49; _LL53: goto _LL49; _LL55: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL49:;}(* vd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp60=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp60[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp61; _temp61.tag= Cyc_Absyn_Abs_n; _temp61.f1= te->ns; _temp61;}); _temp60;});{
void* _temp62= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp69; struct
Cyc_Absyn_Tqual _temp71; void* _temp73; _LL64: if(( unsigned int) _temp62 > 4u?*((
int*) _temp62) == Cyc_Absyn_ArrayType: 0){ _LL74: _temp73=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp62)->f1; goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_ArrayType_struct*)
_temp62)->f2; goto _LL70; _LL70: _temp69=(( struct Cyc_Absyn_ArrayType_struct*)
_temp62)->f3; if( _temp69 == 0){ goto _LL68;} else{ goto _LL66;}} else{ goto
_LL66;} _LL68: if( vd->initializer != 0){ goto _LL65;} else{ goto _LL66;} _LL66:
goto _LL67; _LL65:{ void* _temp75=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp85; struct _tagged_arr _temp87; struct Cyc_List_List*
_temp89; struct Cyc_Core_Opt* _temp91; struct Cyc_List_List* _temp93; _LL77: if(*((
int*) _temp75) == Cyc_Absyn_Const_e){ _LL86: _temp85=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp75)->f1; if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Absyn_String_c:
0){ _LL88: _temp87=(( struct Cyc_Absyn_String_c_struct*) _temp85)->f1; goto
_LL78;} else{ goto _LL79;}} else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==
Cyc_Absyn_UnresolvedMem_e){ _LL92: _temp91=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp75)->f1; goto _LL90; _LL90: _temp89=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp75)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==
Cyc_Absyn_Array_e){ _LL94: _temp93=(( struct Cyc_Absyn_Array_e_struct*) _temp75)->f1;
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL78: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp95=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp95[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp96; _temp96.tag= Cyc_Absyn_ArrayType; _temp96.f1=(
void*) _temp73; _temp96.f2= _temp71; _temp96.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp87, sizeof( unsigned char)), 0); _temp96;}); _temp95;})));
goto _LL76; _LL80: _temp93= _temp89; goto _LL82; _LL82: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp97=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp97[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp98; _temp98.tag= Cyc_Absyn_ArrayType; _temp98.f1=(
void*) _temp73; _temp98.f2= _temp71; _temp98.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp93), 0);
_temp98;}); _temp97;}))); goto _LL76; _LL84: goto _LL76; _LL76:;} goto _LL63;
_LL67: goto _LL63; _LL63:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc, _tag_arr("extern declaration should not have initializer",
sizeof( unsigned char), 47u));}} else{ if( ! Cyc_Tcutil_is_function_type( t)){
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp99=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int _temp119;
struct _tagged_arr _temp121; _LL101: if(( unsigned int) _temp99 > 16u?*(( int*)
_temp99) == Cyc_Absyn_Aligned_att: 0){ _LL120: _temp119=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp99)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int)
_temp99 > 16u?*(( int*) _temp99) == Cyc_Absyn_Section_att: 0){ _LL122: _temp121=((
struct Cyc_Absyn_Section_att_struct*) _temp99)->f1; goto _LL104;} else{ goto
_LL105;} _LL105: if( _temp99 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL106;}
else{ goto _LL107;} _LL107: if( _temp99 ==( void*) Cyc_Absyn_Shared_att){ goto
_LL108;} else{ goto _LL109;} _LL109: if( _temp99 ==( void*) Cyc_Absyn_Unused_att){
goto _LL110;} else{ goto _LL111;} _LL111: if( _temp99 ==( void*) Cyc_Absyn_Weak_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp99 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp99 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL102: continue; _LL104:
continue; _LL106: continue; _LL108: continue; _LL110: continue; _LL112:
continue; _LL114: continue; _LL116: continue; _LL118: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp123= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); struct _tagged_arr
_temp124= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_get_arr_size( _temp123, 1u), _temp123.curr, _get_arr_size( _temp124, 1u),
_temp124.curr);})); goto _LL100; _LL100:;} if( vd->initializer == 0){ if(
check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp125= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_arr _temp126= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_get_arr_size( _temp125, 1u), _temp125.curr, _get_arr_size( _temp126, 1u),
_temp126.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp127=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); goto _LL128; _LL128: { void* _temp129= Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, _temp127); goto _LL130; _LL130: if( ! Cyc_Tcutil_coerce_assign(
te, _temp127, t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp131= Cyc_Absynpp_qvar2string( vd->name); struct _tagged_arr
_temp132= Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp133= Cyc_Absynpp_typ2string(
_temp129); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_get_arr_size( _temp131, 1u), _temp131.curr, _get_arr_size( _temp132, 1u),
_temp132.curr, _get_arr_size( _temp133, 1u), _temp133.curr);}));} if( ! Cyc_Tcutil_is_const_exp(
te, _temp127)){ Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u));}}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp134=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp154; int _temp156; _LL136: if(( unsigned int)
_temp134 > 16u?*(( int*) _temp134) == Cyc_Absyn_Regparm_att: 0){ _LL155:
_temp154=(( struct Cyc_Absyn_Regparm_att_struct*) _temp134)->f1; goto _LL137;}
else{ goto _LL138;} _LL138: if( _temp134 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL139;} else{ goto _LL140;} _LL140: if( _temp134 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL141;} else{ goto _LL142;} _LL142: if( _temp134 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL143;} else{ goto _LL144;} _LL144: if( _temp134 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL145;} else{ goto _LL146;} _LL146: if( _temp134 ==( void*) Cyc_Absyn_Const_att){
goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp134 > 16u?*((
int*) _temp134) == Cyc_Absyn_Aligned_att: 0){ _LL157: _temp156=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp134)->f1; goto _LL149;} else{ goto _LL150;} _LL150: if( _temp134 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL151;} else{ goto _LL152;} _LL152: goto _LL153;
_LL137: goto _LL139; _LL139: goto _LL141; _LL141: goto _LL143; _LL143: goto
_LL145; _LL145: goto _LL147; _LL147:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcVardecl: fn type attributes in function var decl", sizeof(
unsigned char), 51u)); goto _LL135; _LL149: goto _LL151; _LL151: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp158= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_get_arr_size( _temp158, 1u), _temp158.curr);})); goto _LL135; _LL153: continue;
_LL135:;}}}{ struct _handler_cons _temp159; _push_handler(& _temp159);{ int
_temp161= 0; if( setjmp( _temp159.handler)){ _temp161= 1;} if( ! _temp161){{
struct _tuple3* _temp162=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL163; _LL163: {
void* _temp164=(* _temp162).f1; void* _temp174; struct Cyc_Absyn_Structdecl*
_temp176; struct Cyc_Absyn_Tunionfield* _temp178; struct Cyc_Absyn_Tuniondecl*
_temp180; struct Cyc_Absyn_Enumfield* _temp182; struct Cyc_Absyn_Enumdecl*
_temp184; _LL166: if(*(( int*) _temp164) == Cyc_Tcenv_VarRes){ _LL175: _temp174=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp164)->f1; goto _LL167;} else{
goto _LL168;} _LL168: if(*(( int*) _temp164) == Cyc_Tcenv_StructRes){ _LL177:
_temp176=(( struct Cyc_Tcenv_StructRes_struct*) _temp164)->f1; goto _LL169;}
else{ goto _LL170;} _LL170: if(*(( int*) _temp164) == Cyc_Tcenv_TunionRes){
_LL181: _temp180=(( struct Cyc_Tcenv_TunionRes_struct*) _temp164)->f1; goto
_LL179; _LL179: _temp178=(( struct Cyc_Tcenv_TunionRes_struct*) _temp164)->f2;
goto _LL171;} else{ goto _LL172;} _LL172: if(*(( int*) _temp164) == Cyc_Tcenv_EnumRes){
_LL185: _temp184=(( struct Cyc_Tcenv_EnumRes_struct*) _temp164)->f1; goto _LL183;
_LL183: _temp182=(( struct Cyc_Tcenv_EnumRes_struct*) _temp164)->f2; goto _LL173;}
else{ goto _LL165;} _LL167: { struct Cyc_Absyn_Global_b_struct* _temp188=({
struct Cyc_Absyn_Global_b_struct* _temp186=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp186[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp187; _temp187.tag= Cyc_Absyn_Global_b; _temp187.f1=
vd; _temp187;}); _temp186;}); goto _LL189; _LL189: { void* _temp190= Cyc_Tcdecl_merge_binding(
_temp174,( void*) _temp188, loc, Cyc_Tc_tc_msg); goto _LL191; _LL191: if(
_temp190 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL165;} if( _temp190 ==
_temp174?(* _temp162).f2: 0){ goto _LL165;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp192=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp192->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp193=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp193[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp194; _temp194.tag= Cyc_Tcenv_VarRes;
_temp194.f1=( void*) _temp190; _temp194;}); _temp193;}); _temp192->f2= 1;
_temp192;})); goto _LL165;}} _LL169: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL165; _LL171: Cyc_Tcutil_warn( loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL165; _LL173: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL165; _LL165:;}}; _pop_handler();} else{
void* _temp160=( void*) _exn_thrown; void* _temp196= _temp160; _LL198: if(
_temp196 == Cyc_Dict_Absent){ goto _LL199;} else{ goto _LL200;} _LL200: goto
_LL201; _LL199: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp202=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp202->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp203=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp203[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp204; _temp204.tag= Cyc_Tcenv_VarRes;
_temp204.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp205=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp205[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp206; _temp206.tag= Cyc_Absyn_Global_b;
_temp206.f1= vd; _temp206;}); _temp205;})); _temp204;}); _temp203;}); _temp202->f2=
0; _temp202;})); goto _LL197; _LL201:( void) _throw( _temp196); _LL197:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp207=(* fd->name).f1;
struct Cyc_List_List* _temp215; struct Cyc_List_List* _temp217; _LL209: if((
unsigned int) _temp207 > 1u?*(( int*) _temp207) == Cyc_Absyn_Rel_n: 0){ _LL216:
_temp215=(( struct Cyc_Absyn_Rel_n_struct*) _temp207)->f1; if( _temp215 == 0){
goto _LL210;} else{ goto _LL211;}} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp207 > 1u?*(( int*) _temp207) == Cyc_Absyn_Abs_n: 0){ _LL218: _temp217=((
struct Cyc_Absyn_Abs_n_struct*) _temp207)->f1; goto _LL212;} else{ goto _LL213;}
_LL213: goto _LL214; _LL210: goto _LL208; _LL212:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp219=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp219[ 0]=({ struct
Cyc_Core_Impossible_struct _temp220; _temp220.tag= Cyc_Core_Impossible; _temp220.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp220;});
_temp219;})); _LL214: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL208:;}(* fd->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp221=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp221[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp222; _temp222.tag= Cyc_Absyn_Abs_n; _temp222.f1= te->ns; _temp222;});
_temp221;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp223=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp231; _LL225: if(
_temp223 ==( void*) Cyc_Absyn_Packed_att){ goto _LL226;} else{ goto _LL227;}
_LL227: if(( unsigned int) _temp223 > 16u?*(( int*) _temp223) == Cyc_Absyn_Aligned_att:
0){ _LL232: _temp231=(( struct Cyc_Absyn_Aligned_att_struct*) _temp223)->f1;
goto _LL228;} else{ goto _LL229;} _LL229: goto _LL230; _LL226: goto _LL228;
_LL228: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp233=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s for function", _get_arr_size( _temp233, 1u),
_temp233.curr);})); goto _LL224; _LL230: goto _LL224; _LL224:;}}{ struct
_handler_cons _temp234; _push_handler(& _temp234);{ int _temp236= 0; if( setjmp(
_temp234.handler)){ _temp236= 1;} if( ! _temp236){{ struct _tuple3* _temp237=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); goto _LL238; _LL238: { void* _temp239=(* _temp237).f1; void*
_temp249; struct Cyc_Absyn_Structdecl* _temp251; struct Cyc_Absyn_Tunionfield*
_temp253; struct Cyc_Absyn_Tuniondecl* _temp255; struct Cyc_Absyn_Enumfield*
_temp257; struct Cyc_Absyn_Enumdecl* _temp259; _LL241: if(*(( int*) _temp239) ==
Cyc_Tcenv_VarRes){ _LL250: _temp249=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp239)->f1; goto _LL242;} else{ goto _LL243;} _LL243: if(*(( int*) _temp239)
== Cyc_Tcenv_StructRes){ _LL252: _temp251=(( struct Cyc_Tcenv_StructRes_struct*)
_temp239)->f1; goto _LL244;} else{ goto _LL245;} _LL245: if(*(( int*) _temp239)
== Cyc_Tcenv_TunionRes){ _LL256: _temp255=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp239)->f1; goto _LL254; _LL254: _temp253=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp239)->f2; goto _LL246;} else{ goto _LL247;} _LL247: if(*(( int*) _temp239)
== Cyc_Tcenv_EnumRes){ _LL260: _temp259=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp239)->f1; goto _LL258; _LL258: _temp257=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp239)->f2; goto _LL248;} else{ goto _LL240;} _LL242: { struct Cyc_Absyn_Funname_b_struct*
_temp263=({ struct Cyc_Absyn_Funname_b_struct* _temp261=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp261[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp262; _temp262.tag= Cyc_Absyn_Funname_b; _temp262.f1=
fd; _temp262;}); _temp261;}); goto _LL264; _LL264: { void* _temp265= Cyc_Tcdecl_merge_binding(
_temp249,( void*) _temp263, loc, Cyc_Tc_tc_msg); goto _LL266; _LL266: if(
_temp265 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL240;} if( _temp265 ==
_temp249?(* _temp237).f2: 0){ goto _LL240;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp267=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp267->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp268=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp268[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp269; _temp269.tag= Cyc_Tcenv_VarRes;
_temp269.f1=( void*) _temp265; _temp269;}); _temp268;}); _temp267->f2= 1;
_temp267;})); goto _LL240;}} _LL244: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL240; _LL246: Cyc_Tcutil_warn( loc,
_tag_arr("function declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL240; _LL248: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL240; _LL240:;}}; _pop_handler();} else{
void* _temp235=( void*) _exn_thrown; void* _temp271= _temp235; _LL273: if(
_temp271 == Cyc_Dict_Absent){ goto _LL274;} else{ goto _LL275;} _LL275: goto
_LL276; _LL274: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp277=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp277->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp278=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp278[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp279; _temp279.tag= Cyc_Tcenv_VarRes;
_temp279.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp280=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp280[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp281; _temp281.tag= Cyc_Absyn_Funname_b;
_temp281.f1= fd; _temp281;}); _temp280;})); _temp279;}); _temp278;}); _temp277->f2=
0; _temp277;})); goto _LL272; _LL276:( void) _throw( _temp271); _LL272:;}}} te->le=({
struct Cyc_Core_Opt* _temp282=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp282->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp283=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp283[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp284; _temp284.tag= Cyc_Tcenv_Outermost;
_temp284.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp284;}); _temp283;}));
_temp282;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u));} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp285=(* td->name).f1; struct Cyc_List_List* _temp293; struct Cyc_List_List*
_temp295; _LL287: if(( unsigned int) _temp285 > 1u?*(( int*) _temp285) == Cyc_Absyn_Rel_n:
0){ _LL294: _temp293=(( struct Cyc_Absyn_Rel_n_struct*) _temp285)->f1; if(
_temp293 == 0){ goto _LL288;} else{ goto _LL289;}} else{ goto _LL289;} _LL289:
if(( unsigned int) _temp285 > 1u?*(( int*) _temp285) == Cyc_Absyn_Abs_n: 0){
_LL296: _temp295=(( struct Cyc_Absyn_Abs_n_struct*) _temp285)->f1; if( _temp295
== 0){ goto _LL290;} else{ goto _LL291;}} else{ goto _LL291;} _LL291: goto
_LL292; _LL288: goto _LL286; _LL290: goto _LL286; _LL292: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL286:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp297=* v; xprintf("redeclaration of typedef %.*s",
_get_arr_size( _temp297, 1u), _temp297.curr);})); return;}(* td->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp298=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp298[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp299; _temp299.tag= Cyc_Absyn_Abs_n; _temp299.f1= te->ns; _temp299;});
_temp298;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_add_tvar_identities(
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,(
void*) td->defn); ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
ge->typedefs, v, td);} static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, struct Cyc_List_List* fields, struct Cyc_List_List* tvs){
struct _RegionHandle _temp300= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp300; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* _temp301= fields; goto _LL302; _LL302: for( 0; _temp301 !=
0; _temp301=(( struct Cyc_List_List*) _check_null( _temp301))->tl){ struct Cyc_Absyn_Structfield
_temp305; struct Cyc_List_List* _temp306; struct Cyc_Core_Opt* _temp308; void*
_temp310; struct Cyc_Absyn_Tqual _temp312; struct _tagged_arr* _temp314; struct
Cyc_Absyn_Structfield* _temp303=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp301))->hd; _temp305=* _temp303; _LL315: _temp314= _temp305.name;
goto _LL313; _LL313: _temp312= _temp305.tq; goto _LL311; _LL311: _temp310=( void*)
_temp305.type; goto _LL309; _LL309: _temp308= _temp305.width; goto _LL307;
_LL307: _temp306= _temp305.attributes; goto _LL304; _LL304: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp314)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp316=* _temp314; struct _tagged_arr _temp317= obj; xprintf("duplicate field %.*s in %.*s",
_get_arr_size( _temp316, 1u), _temp316.curr, _get_arr_size( _temp317, 1u),
_temp317.curr);}));} if( Cyc_String_strcmp(* _temp314, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List* _temp318=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp318->hd=( void*) _temp314; _temp318->tl= prev_fields; _temp318;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp310); if( _temp308 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp308))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp319=* _temp314; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp319, 1u), _temp319.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp308))->v);}{
void* _temp320= Cyc_Tcutil_compress( _temp310); void* _temp326; void* _temp328;
_LL322: if(( unsigned int) _temp320 > 4u?*(( int*) _temp320) == Cyc_Absyn_IntType:
0){ _LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp320)->f1;
goto _LL327; _LL327: _temp326=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp320)->f2; goto _LL323;} else{ goto _LL324;} _LL324: goto _LL325; _LL323:{
void* _temp330= _temp326; _LL332: if( _temp330 ==( void*) Cyc_Absyn_B1){ goto
_LL333;} else{ goto _LL334;} _LL334: if( _temp330 ==( void*) Cyc_Absyn_B2){ goto
_LL335;} else{ goto _LL336;} _LL336: if( _temp330 ==( void*) Cyc_Absyn_B4){ goto
_LL337;} else{ goto _LL338;} _LL338: if( _temp330 ==( void*) Cyc_Absyn_B8){ goto
_LL339;} else{ goto _LL331;} _LL333: if( w > 8){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL331; _LL335: if( w > 16){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL331; _LL337: if( w > 32){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL331; _LL339: if( w > 64){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL331; _LL331:;} goto _LL321; _LL325: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp340=* _temp314; struct _tagged_arr _temp341= Cyc_Absynpp_typ2string(
_temp310); xprintf("bitfield %.*s must have integral type but has type %.*s",
_get_arr_size( _temp340, 1u), _temp340.curr, _get_arr_size( _temp341, 1u),
_temp341.curr);})); goto _LL321; _LL321:;}}}}; _pop_region( uprev_rgn);} static
void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr obj, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl*
sd){ struct _tagged_arr* _temp342=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; goto _LL343; _LL343: if( sd->name == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp344= obj; xprintf("anonymous %.*ss are not allowed at top level",
_get_arr_size( _temp344, 1u), _temp344.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp345=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp353; _LL347: if( _temp345 ==( void*) Cyc_Absyn_Packed_att){
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp345 > 16u?*((
int*) _temp345) == Cyc_Absyn_Aligned_att: 0){ _LL354: _temp353=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp345)->f1; goto _LL350;} else{ goto _LL351;} _LL351: goto _LL352; _LL348:
continue; _LL350: continue; _LL352: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp355= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_arr _temp356= obj; struct _tagged_arr
_temp357=* _temp342; xprintf("bad attribute %.*s in %.*s %.*s definition",
_get_arr_size( _temp355, 1u), _temp355.curr, _get_arr_size( _temp356, 1u),
_temp356.curr, _get_arr_size( _temp357, 1u), _temp357.curr);})); goto _LL346;
_LL346:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0; tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref* _temp358=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL359; _LL359: { void* _temp360=( void*) _temp358->v; void* _temp368;
_LL362: if( _temp360 ==( void*) Cyc_Absyn_No_constr){ goto _LL363;} else{ goto
_LL364;} _LL364: if(( unsigned int) _temp360 > 1u?*(( int*) _temp360) == Cyc_Absyn_Eq_constr:
0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp360)->f1;
if( _temp368 ==( void*) Cyc_Absyn_MemKind){ goto _LL365;} else{ goto _LL366;}}
else{ goto _LL366;} _LL366: goto _LL367; _LL363:( void*)( _temp358->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp370=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp370[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp371; _temp371.tag= Cyc_Absyn_Eq_constr; _temp371.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp371;}); _temp370;}))); continue; _LL365:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp372= obj;
struct _tagged_arr _temp373=* _temp342; struct _tagged_arr _temp374=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp372, 1u), _temp372.curr, _get_arr_size( _temp373, 1u),
_temp373.curr, _get_arr_size( _temp374, 1u), _temp374.curr);})); continue;
_LL367: continue; _LL361:;}}}{ void* _temp375=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp383; struct Cyc_List_List*
_temp385; _LL377: if(( unsigned int) _temp375 > 1u?*(( int*) _temp375) == Cyc_Absyn_Rel_n:
0){ _LL384: _temp383=(( struct Cyc_Absyn_Rel_n_struct*) _temp375)->f1; if(
_temp383 == 0){ goto _LL378;} else{ goto _LL379;}} else{ goto _LL379;} _LL379:
if(( unsigned int) _temp375 > 1u?*(( int*) _temp375) == Cyc_Absyn_Abs_n: 0){
_LL386: _temp385=(( struct Cyc_Absyn_Abs_n_struct*) _temp375)->f1; if( _temp385
== 0){ goto _LL380;} else{ goto _LL381;}} else{ goto _LL381;} _LL381: goto
_LL382; _LL378: goto _LL376; _LL380: goto _LL376; _LL382: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL376:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp387=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp387[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp388; _temp388.tag= Cyc_Absyn_Abs_n;
_temp388.f1= te->ns; _temp388;}); _temp387;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp389=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto _LL390;
_LL390: { const unsigned char* _temp391="struct"; goto _LL392; _LL392: Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp391, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp393= sd->tvs; goto _LL394; _LL394:{ struct _tuple4 _temp396=({ struct
_tuple4 _temp395; _temp395.f1= sd->fields; _temp395.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls,
_temp389); _temp395;}); struct Cyc_Core_Opt* _temp406; struct Cyc_Core_Opt*
_temp408; struct Cyc_Core_Opt* _temp410; struct Cyc_Core_Opt* _temp412; struct
Cyc_Core_Opt _temp414; struct Cyc_List_List* _temp415; struct Cyc_Core_Opt*
_temp417; struct Cyc_Core_Opt _temp419; struct Cyc_Absyn_Structdecl** _temp420;
struct Cyc_Core_Opt* _temp422; struct Cyc_Core_Opt _temp424; struct Cyc_List_List*
_temp425; struct Cyc_Core_Opt* _temp427; struct Cyc_Core_Opt _temp429; struct
Cyc_Absyn_Structdecl** _temp430; struct Cyc_Core_Opt* _temp432; _LL398: _LL409:
_temp408= _temp396.f1; if( _temp408 == 0){ goto _LL407;} else{ goto _LL400;}
_LL407: _temp406= _temp396.f2; if( _temp406 == 0){ goto _LL399;} else{ goto
_LL400;} _LL400: _LL413: _temp412= _temp396.f1; if( _temp412 == 0){ goto _LL402;}
else{ _temp414=* _temp412; _LL416: _temp415=( struct Cyc_List_List*) _temp414.v;
goto _LL411;} _LL411: _temp410= _temp396.f2; if( _temp410 == 0){ goto _LL401;}
else{ goto _LL402;} _LL402: _LL423: _temp422= _temp396.f1; if( _temp422 == 0){
goto _LL404;} else{ _temp424=* _temp422; _LL426: _temp425=( struct Cyc_List_List*)
_temp424.v; goto _LL418;} _LL418: _temp417= _temp396.f2; if( _temp417 == 0){
goto _LL404;} else{ _temp419=* _temp417; _LL421: _temp420=( struct Cyc_Absyn_Structdecl**)
_temp419.v; goto _LL403;} _LL404: _LL433: _temp432= _temp396.f1; if( _temp432 ==
0){ goto _LL428;} else{ goto _LL397;} _LL428: _temp427= _temp396.f2; if(
_temp427 == 0){ goto _LL397;} else{ _temp429=* _temp427; _LL431: _temp430=(
struct Cyc_Absyn_Structdecl**) _temp429.v; goto _LL405;} _LL399: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, _temp389,({
struct Cyc_Absyn_Structdecl** _temp434=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp434[ 0]= sd; _temp434;}));
goto _LL397; _LL401: { struct Cyc_Absyn_Structdecl** _temp437=({ struct Cyc_Absyn_Structdecl**
_temp435=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp435[ 0]=({ struct Cyc_Absyn_Structdecl* _temp436=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp436->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp436->name= sd->name; _temp436->tvs= _temp393; _temp436->fields=
0; _temp436->attributes= 0; _temp436;}); _temp435;}); goto _LL438; _LL438: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, _temp389,
_temp437); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp391, sizeof(
unsigned char), 7u), _temp415, _temp393);* _temp437= sd; goto _LL397;} _LL403: {
struct Cyc_Absyn_Structdecl* _temp439=* _temp420; goto _LL440; _LL440:* _temp420=({
struct Cyc_Absyn_Structdecl* _temp441=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp441->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp441->name= sd->name; _temp441->tvs= _temp393; _temp441->fields= 0; _temp441->attributes=
0; _temp441;}); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp391, sizeof(
unsigned char), 7u), _temp425, _temp393);* _temp420= _temp439; _temp430=
_temp420; goto _LL405;} _LL405: { struct Cyc_Absyn_Structdecl* _temp442= Cyc_Tcdecl_merge_structdecl(*
_temp430, sd, loc, Cyc_Tc_tc_msg); goto _LL443; _LL443: if( _temp442 == 0){
return;} else{* _temp430=( struct Cyc_Absyn_Structdecl*) _check_null( _temp442);
sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp442); goto _LL397;}} _LL397:;}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp389,({ struct _tuple3* _temp444=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp444->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp445=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp445[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp446; _temp446.tag= Cyc_Tcenv_StructRes;
_temp446.f1= sd; _temp446;}); _temp445;}); _temp444->f2= 1; _temp444;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp447=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; goto _LL448; _LL448: { const unsigned char* _temp449="union";
goto _LL450; _LL450: Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr( _temp449,
sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*) ud);{ struct
Cyc_List_List* _temp451= ud->tvs; goto _LL452; _LL452: { struct _tuple4 _temp454=({
struct _tuple4 _temp453; _temp453.f1= ud->fields; _temp453.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp447); _temp453;}); struct Cyc_Core_Opt* _temp464; struct Cyc_Core_Opt*
_temp466; struct Cyc_Core_Opt* _temp468; struct Cyc_Core_Opt* _temp470; struct
Cyc_Core_Opt _temp472; struct Cyc_List_List* _temp473; struct Cyc_Core_Opt*
_temp475; struct Cyc_Core_Opt _temp477; struct Cyc_Absyn_Uniondecl** _temp478;
struct Cyc_Core_Opt* _temp480; struct Cyc_Core_Opt _temp482; struct Cyc_List_List*
_temp483; struct Cyc_Core_Opt* _temp485; struct Cyc_Core_Opt _temp487; struct
Cyc_Absyn_Uniondecl** _temp488; struct Cyc_Core_Opt* _temp490; _LL456: _LL467:
_temp466= _temp454.f1; if( _temp466 == 0){ goto _LL465;} else{ goto _LL458;}
_LL465: _temp464= _temp454.f2; if( _temp464 == 0){ goto _LL457;} else{ goto
_LL458;} _LL458: _LL471: _temp470= _temp454.f1; if( _temp470 == 0){ goto _LL460;}
else{ _temp472=* _temp470; _LL474: _temp473=( struct Cyc_List_List*) _temp472.v;
goto _LL469;} _LL469: _temp468= _temp454.f2; if( _temp468 == 0){ goto _LL459;}
else{ goto _LL460;} _LL460: _LL481: _temp480= _temp454.f1; if( _temp480 == 0){
goto _LL462;} else{ _temp482=* _temp480; _LL484: _temp483=( struct Cyc_List_List*)
_temp482.v; goto _LL476;} _LL476: _temp475= _temp454.f2; if( _temp475 == 0){
goto _LL462;} else{ _temp477=* _temp475; _LL479: _temp478=( struct Cyc_Absyn_Uniondecl**)
_temp477.v; goto _LL461;} _LL462: _LL491: _temp490= _temp454.f1; if( _temp490 ==
0){ goto _LL486;} else{ goto _LL455;} _LL486: _temp485= _temp454.f2; if(
_temp485 == 0){ goto _LL455;} else{ _temp487=* _temp485; _LL489: _temp488=(
struct Cyc_Absyn_Uniondecl**) _temp487.v; goto _LL463;} _LL457: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp447,({
struct Cyc_Absyn_Uniondecl** _temp492=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp492[ 0]= ud; _temp492;})); goto
_LL455; _LL459: { struct Cyc_Absyn_Uniondecl** _temp495=({ struct Cyc_Absyn_Uniondecl**
_temp493=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp493[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp494=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp494->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp494->name= ud->name; _temp494->tvs= _temp451; _temp494->fields=
0; _temp494->attributes= ud->attributes; _temp494;}); _temp493;}); goto _LL496;
_LL496: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp447, _temp495); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp449, sizeof( unsigned char), 6u), _temp473, _temp451);{ struct
Cyc_List_List* f= _temp473; for( 0; f != 0; f=(( struct Cyc_List_List*)
_check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp497=*(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->name; struct _tagged_arr _temp498=*
_temp447; struct _tagged_arr _temp499= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);
xprintf("field %.*s of union %.*s has type %.*s which is not `bits-only'",
_get_arr_size( _temp497, 1u), _temp497.curr, _get_arr_size( _temp498, 1u),
_temp498.curr, _get_arr_size( _temp499, 1u), _temp499.curr);}));}}}* _temp495=
ud; goto _LL455;} _LL461: { struct Cyc_Absyn_Uniondecl* _temp500=* _temp478;
goto _LL501; _LL501:* _temp478=({ struct Cyc_Absyn_Uniondecl* _temp502=( struct
Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp502->sc=(
void*)(( void*) Cyc_Absyn_Extern); _temp502->name= ud->name; _temp502->tvs=
_temp451; _temp502->fields= 0; _temp502->attributes= ud->attributes; _temp502;});
Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp449, sizeof( unsigned char),
6u), _temp483, _temp451);* _temp478= _temp500; _temp488= _temp478; goto _LL463;}
_LL463: { struct Cyc_Absyn_Uniondecl* _temp503= Cyc_Tcdecl_merge_uniondecl(*
_temp488, ud, loc, Cyc_Tc_tc_msg); goto _LL504; _LL504: if( _temp503 == 0){
return;} else{* _temp488=( struct Cyc_Absyn_Uniondecl*) _check_null( _temp503);
goto _LL455;}} _LL455:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp505= fields; goto _LL506; _LL506: for( 0; _temp505 != 0;
_temp505=(( struct Cyc_List_List*) _check_null( _temp505))->tl){ struct Cyc_Absyn_Tunionfield*
_temp507=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp505))->hd; goto _LL508; _LL508:{ struct Cyc_List_List* tvs= _temp507->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp509=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL510; _LL510: { struct Cyc_Absyn_Conref* _temp511=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp509->kind); goto _LL512; _LL512: { void* _temp513=( void*) _temp511->v;
void* _temp521; _LL515: if( _temp513 ==( void*) Cyc_Absyn_No_constr){ goto
_LL516;} else{ goto _LL517;} _LL517: if(( unsigned int) _temp513 > 1u?*(( int*)
_temp513) == Cyc_Absyn_Eq_constr: 0){ _LL522: _temp521=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp513)->f1; if( _temp521 ==( void*) Cyc_Absyn_MemKind){ goto _LL518;} else{
goto _LL519;}} else{ goto _LL519;} _LL519: goto _LL520; _LL516:( void*)(
_temp511->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp523=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp523[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp524; _temp524.tag= Cyc_Absyn_Eq_constr;
_temp524.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp524;}); _temp523;})));
goto _LL514; _LL518: Cyc_Tcutil_terr( _temp507->loc,( struct _tagged_arr)({
struct _tagged_arr _temp525=*(* _temp507->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_get_arr_size( _temp525, 1u), _temp525.curr);})); goto _LL514; _LL520: goto
_LL514; _LL514:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp507->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp507->tvs);{ struct Cyc_List_List* typs= _temp507->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp507->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp526=(*
_temp507->name).f1; struct Cyc_List_List* _temp536; struct Cyc_List_List*
_temp538; struct Cyc_List_List* _temp540; _LL528: if(( unsigned int) _temp526 >
1u?*(( int*) _temp526) == Cyc_Absyn_Rel_n: 0){ _LL537: _temp536=(( struct Cyc_Absyn_Rel_n_struct*)
_temp526)->f1; if( _temp536 == 0){ goto _LL529;} else{ goto _LL530;}} else{ goto
_LL530;} _LL530: if(( unsigned int) _temp526 > 1u?*(( int*) _temp526) == Cyc_Absyn_Rel_n:
0){ _LL539: _temp538=(( struct Cyc_Absyn_Rel_n_struct*) _temp526)->f1; goto
_LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp526 > 1u?*(( int*)
_temp526) == Cyc_Absyn_Abs_n: 0){ _LL541: _temp540=(( struct Cyc_Absyn_Abs_n_struct*)
_temp526)->f1; goto _LL533;} else{ goto _LL534;} _LL534: if( _temp526 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL535;} else{ goto _LL527;} _LL529: if( is_xtunion){(*
_temp507->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp542=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp542[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp543; _temp543.tag= Cyc_Absyn_Abs_n;
_temp543.f1= te->ns; _temp543;}); _temp542;});} else{(* _temp507->name).f1=(*
name).f1;} goto _LL527; _LL531: Cyc_Tcutil_terr( _temp507->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u)); goto _LL527; _LL533: goto _LL527; _LL535:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp544=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp544[ 0]=({ struct
Cyc_Core_Impossible_struct _temp545; _temp545.tag= Cyc_Core_Impossible; _temp545.f1=
_tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u); _temp545;});
_temp544;})); goto _LL527; _LL527:;}}}}{ struct Cyc_List_List* fields2; if(
is_xtunion){ int _temp546= 1; goto _LL547; _LL547: { struct Cyc_List_List*
_temp548= Cyc_Tcdecl_sort_xtunion_fields( fields,& _temp546,(* name).f2, loc,
Cyc_Tc_tc_msg); goto _LL549; _LL549: if( _temp546){ fields2= _temp548;} else{
fields2= 0;}}} else{ struct _RegionHandle _temp550= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp550; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield* _temp551=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL552; _LL552: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp551->name).f2)){ Cyc_Tcutil_terr(
_temp551->loc,( struct _tagged_arr)({ struct _tagged_arr _temp553=*(* _temp551->name).f2;
struct _tagged_arr _temp554= obj; xprintf("duplicate field name %.*s in %.*s",
_get_arr_size( _temp553, 1u), _temp553.curr, _get_arr_size( _temp554, 1u),
_temp554.curr);}));} else{ prev_fields=({ struct Cyc_List_List* _temp555=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp555->hd=( void*)(* _temp551->name).f2; _temp555->tl= prev_fields; _temp555;});}
if(( void*) _temp551->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp556=*(* _temp551->name).f2;
xprintf("ignoring scope of field %.*s", _get_arr_size( _temp556, 1u), _temp556.curr);}));(
void*)( _temp551->sc=( void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;};
_pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp557= fields; goto _LL558;
_LL558: for( 0; _temp557 != 0; _temp557=(( struct Cyc_List_List*) _check_null(
_temp557))->tl){ struct Cyc_Absyn_Tunionfield* _temp559=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp557))->hd; goto _LL560; _LL560: ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(* _temp559->name).f2,({
struct _tuple3* _temp561=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp561->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp562=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp562[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp563; _temp563.tag= Cyc_Tcenv_TunionRes; _temp563.f1=
tudres; _temp563.f2= _temp559; _temp563;}); _temp562;}); _temp561->f2= 1;
_temp561;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->kind); void* _temp564=( void*) c->v; void* _temp572;
_LL566: if( _temp564 ==( void*) Cyc_Absyn_No_constr){ goto _LL567;} else{ goto
_LL568;} _LL568: if(( unsigned int) _temp564 > 1u?*(( int*) _temp564) == Cyc_Absyn_Eq_constr:
0){ _LL573: _temp572=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp564)->f1;
if( _temp572 ==( void*) Cyc_Absyn_MemKind){ goto _LL569;} else{ goto _LL570;}}
else{ goto _LL570;} _LL570: goto _LL571; _LL567:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp574=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp574[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp575; _temp575.tag= Cyc_Absyn_Eq_constr; _temp575.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp575;}); _temp574;}))); goto _LL565;
_LL569: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp576=
obj; struct _tagged_arr _temp577=* v; struct _tagged_arr _temp578=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp576, 1u), _temp576.curr, _get_arr_size( _temp577, 1u),
_temp577.curr, _get_arr_size( _temp578, 1u), _temp578.curr);})); goto _LL565;
_LL571: goto _LL565; _LL565:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp579; _push_handler(& _temp579);{ int _temp581= 0; if( setjmp( _temp579.handler)){
_temp581= 1;} if( ! _temp581){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp580=( void*) _exn_thrown; void*
_temp583= _temp580; _LL585: if( _temp583 == Cyc_Dict_Absent){ goto _LL586;}
else{ goto _LL587;} _LL587: goto _LL588; _LL586: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp589= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_get_arr_size( _temp589, 1u), _temp589.curr);})); return; _LL588:( void) _throw(
_temp583); _LL584:;}}} if( tud_opt != 0){ tud->name=(*(( struct Cyc_Absyn_Tuniondecl**)((
struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp590=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp590[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp591; _temp591.tag= Cyc_Absyn_Abs_n; _temp591.f1= te->ns; _temp591;});
_temp590;});}} else{{ void* _temp592=(* tud->name).f1; struct Cyc_List_List*
_temp600; struct Cyc_List_List* _temp602; _LL594: if(( unsigned int) _temp592 >
1u?*(( int*) _temp592) == Cyc_Absyn_Rel_n: 0){ _LL601: _temp600=(( struct Cyc_Absyn_Rel_n_struct*)
_temp592)->f1; if( _temp600 == 0){ goto _LL595;} else{ goto _LL596;}} else{ goto
_LL596;} _LL596: if(( unsigned int) _temp592 > 1u?*(( int*) _temp592) == Cyc_Absyn_Abs_n:
0){ _LL603: _temp602=(( struct Cyc_Absyn_Abs_n_struct*) _temp592)->f1; goto
_LL597;} else{ goto _LL598;} _LL598: goto _LL599; _LL595:(* tud->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp604=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp605; _temp605.tag= Cyc_Absyn_Abs_n; _temp605.f1= te->ns; _temp605;});
_temp604;}); goto _LL593; _LL597: goto _LL599; _LL599: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL593:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp607=({ struct _tuple4 _temp606; _temp606.f1= tud->fields; _temp606.f2=
tud_opt; _temp606;}); struct Cyc_Core_Opt* _temp617; struct Cyc_Core_Opt*
_temp619; struct Cyc_Core_Opt* _temp621; struct Cyc_Core_Opt* _temp623; struct
Cyc_Core_Opt _temp625; struct Cyc_List_List* _temp626; struct Cyc_List_List**
_temp628; struct Cyc_Core_Opt* _temp629; struct Cyc_Core_Opt _temp631; struct
Cyc_Absyn_Tuniondecl** _temp632; struct Cyc_Core_Opt* _temp634; struct Cyc_Core_Opt
_temp636; struct Cyc_List_List* _temp637; struct Cyc_List_List** _temp639;
struct Cyc_Core_Opt* _temp640; struct Cyc_Core_Opt _temp642; struct Cyc_Absyn_Tuniondecl**
_temp643; struct Cyc_Core_Opt* _temp645; _LL609: _LL620: _temp619= _temp607.f1;
if( _temp619 == 0){ goto _LL618;} else{ goto _LL611;} _LL618: _temp617= _temp607.f2;
if( _temp617 == 0){ goto _LL610;} else{ goto _LL611;} _LL611: _LL624: _temp623=
_temp607.f1; if( _temp623 == 0){ goto _LL613;} else{ _temp625=* _temp623; _LL627:
_temp626=( struct Cyc_List_List*) _temp625.v; _temp628=&(* _temp607.f1).v; goto
_LL622;} _LL622: _temp621= _temp607.f2; if( _temp621 == 0){ goto _LL612;} else{
goto _LL613;} _LL613: _LL635: _temp634= _temp607.f1; if( _temp634 == 0){ goto
_LL615;} else{ _temp636=* _temp634; _LL638: _temp637=( struct Cyc_List_List*)
_temp636.v; _temp639=&(* _temp607.f1).v; goto _LL630;} _LL630: _temp629=
_temp607.f2; if( _temp629 == 0){ goto _LL615;} else{ _temp631=* _temp629; _LL633:
_temp632=( struct Cyc_Absyn_Tuniondecl**) _temp631.v; goto _LL614;} _LL615:
_LL646: _temp645= _temp607.f1; if( _temp645 == 0){ goto _LL641;} else{ goto
_LL608;} _LL641: _temp640= _temp607.f2; if( _temp640 == 0){ goto _LL608;} else{
_temp642=* _temp640; _LL644: _temp643=( struct Cyc_Absyn_Tuniondecl**) _temp642.v;
goto _LL616;} _LL610: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp647=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp647[ 0]= tud; _temp647;}));
goto _LL608; _LL612: { struct Cyc_Absyn_Tuniondecl** _temp650=({ struct Cyc_Absyn_Tuniondecl**
_temp648=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp648[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp649=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp649->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp649->name= tud->name; _temp649->tvs= tvs; _temp649->fields=
0; _temp649->is_xtunion= tud->is_xtunion; _temp649;}); _temp648;}); goto _LL651;
_LL651: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp650);* _temp628= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp628, tvs, tud);* _temp650= tud; goto
_LL608;} _LL614: { struct Cyc_Absyn_Tuniondecl* _temp652=* _temp632; goto _LL653;
_LL653:* _temp632=({ struct Cyc_Absyn_Tuniondecl* _temp654=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp654->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp654->name= tud->name; _temp654->tvs= tvs; _temp654->fields=
0; _temp654->is_xtunion= tud->is_xtunion; _temp654;});* _temp639= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp639, tvs, tud);* _temp632=
_temp652; _temp643= _temp632; goto _LL616;} _LL616: { struct Cyc_Absyn_Tuniondecl*
_temp655= Cyc_Tcdecl_merge_tuniondecl(* _temp643, tud, loc, Cyc_Tc_tc_msg); goto
_LL656; _LL656: if( _temp655 == 0){ return;} else{* _temp643=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp655); goto _LL608;}} _LL608:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_arr* v=(* ed->name).f2;{
void* _temp657=(* ed->name).f1; struct Cyc_List_List* _temp665; struct Cyc_List_List*
_temp667; _LL659: if(( unsigned int) _temp657 > 1u?*(( int*) _temp657) == Cyc_Absyn_Rel_n:
0){ _LL666: _temp665=(( struct Cyc_Absyn_Rel_n_struct*) _temp657)->f1; if(
_temp665 == 0){ goto _LL660;} else{ goto _LL661;}} else{ goto _LL661;} _LL661:
if(( unsigned int) _temp657 > 1u?*(( int*) _temp657) == Cyc_Absyn_Abs_n: 0){
_LL668: _temp667=(( struct Cyc_Absyn_Abs_n_struct*) _temp657)->f1; if( _temp667
== 0){ goto _LL662;} else{ goto _LL663;}} else{ goto _LL663;} _LL663: goto
_LL664; _LL660: goto _LL658; _LL662: goto _LL658; _LL664: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL658:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp669=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp669[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp670; _temp670.tag= Cyc_Absyn_Abs_n;
_temp670.f1= te->ns; _temp670;}); _temp669;}); if( ed->fields != 0){ struct
_RegionHandle _temp671= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp671; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp672=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL673; _LL673: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp672->name).f2)){ Cyc_Tcutil_terr(
_temp672->loc,( struct _tagged_arr)({ struct _tagged_arr _temp674=*(* _temp672->name).f2;
xprintf("duplicate field name %.*s", _get_arr_size( _temp674, 1u), _temp674.curr);}));}
else{ prev_fields=({ struct Cyc_List_List* _temp675=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp675->hd=( void*)(*
_temp672->name).f2; _temp675->tl= prev_fields; _temp675;});} if( _temp672->tag
== 0){ _temp672->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count,
_temp672->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp672->tag))){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp676=* v; struct _tagged_arr _temp677=*(* _temp672->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _get_arr_size(
_temp676, 1u), _temp676.curr, _get_arr_size( _temp677, 1u), _temp677.curr);}));}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp672->tag)); tag_count= t1 + 1;(* _temp672->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp678=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp678[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp679; _temp679.tag= Cyc_Absyn_Abs_n; _temp679.f1= te->ns; _temp679;});
_temp678;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp680;
_push_handler(& _temp680);{ int _temp682= 0; if( setjmp( _temp680.handler)){
_temp682= 1;} if( ! _temp682){{ struct Cyc_Absyn_Enumdecl** _temp683=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); goto _LL684; _LL684: { struct Cyc_Absyn_Enumdecl* _temp685=
Cyc_Tcdecl_merge_enumdecl(* _temp683, ed, loc, Cyc_Tc_tc_msg); goto _LL686;
_LL686: if( _temp685 == 0){ _npop_handler( 0u); return;}* _temp683=( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp685);}}; _pop_handler();} else{ void* _temp681=( void*)
_exn_thrown; void* _temp688= _temp681; _LL690: if( _temp688 == Cyc_Dict_Absent){
goto _LL691;} else{ goto _LL692;} _LL692: goto _LL693; _LL691: { struct Cyc_Absyn_Enumdecl**
_temp695=({ struct Cyc_Absyn_Enumdecl** _temp694=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp694[ 0]= ed; _temp694;});
goto _LL696; _LL696: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v, _temp695); goto _LL689;} _LL693:( void) _throw( _temp688);
_LL689:;}}} if( ed->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp697=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL698; _LL698: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp697->name).f2,({ struct _tuple3* _temp699=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp699->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp700=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp700[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp701; _temp701.tag= Cyc_Tcenv_EnumRes;
_temp701.f1= ed; _temp701.f2= _temp697; _temp701;}); _temp700;}); _temp699->f2=
1; _temp699;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp702= sc; _LL704: if( _temp702 ==( void*) Cyc_Absyn_Static){
goto _LL705;} else{ goto _LL706;} _LL706: if( _temp702 ==( void*) Cyc_Absyn_Abstract){
goto _LL707;} else{ goto _LL708;} _LL708: if( _temp702 ==( void*) Cyc_Absyn_Public){
goto _LL709;} else{ goto _LL710;} _LL710: if( _temp702 ==( void*) Cyc_Absyn_Extern){
goto _LL711;} else{ goto _LL712;} _LL712: if( _temp702 ==( void*) Cyc_Absyn_ExternC){
goto _LL713;} else{ goto _LL703;} _LL705: Cyc_Tcutil_warn( loc, _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u)); return 0; _LL707: Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u)); return 0; _LL709: return 1; _LL711: return 1;
_LL713: Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration",
sizeof( unsigned char), 30u)); return 1; _LL703:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp714= ds0; goto _LL715; _LL715: for( 0; _temp714 != 0;
_temp714=(( struct Cyc_List_List*) _check_null( _temp714))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp714))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp716=( void*) d->r; int
_temp742; struct Cyc_Absyn_Exp* _temp744; struct Cyc_Core_Opt* _temp746; struct
Cyc_Core_Opt* _temp748; struct Cyc_Absyn_Pat* _temp750; struct Cyc_List_List*
_temp752; struct Cyc_Absyn_Vardecl* _temp754; struct Cyc_Absyn_Fndecl* _temp756;
struct Cyc_Absyn_Typedefdecl* _temp758; struct Cyc_Absyn_Structdecl* _temp760;
struct Cyc_Absyn_Uniondecl* _temp762; struct Cyc_Absyn_Tuniondecl* _temp764;
struct Cyc_Absyn_Enumdecl* _temp766; struct Cyc_List_List* _temp768; struct
_tagged_arr* _temp770; struct Cyc_List_List* _temp772; struct _tuple0* _temp774;
struct _tuple0 _temp776; struct _tagged_arr* _temp777; void* _temp779; struct
Cyc_List_List* _temp781; _LL718: if(*(( int*) _temp716) == Cyc_Absyn_Let_d){
_LL751: _temp750=(( struct Cyc_Absyn_Let_d_struct*) _temp716)->f1; goto _LL749;
_LL749: _temp748=(( struct Cyc_Absyn_Let_d_struct*) _temp716)->f2; goto _LL747;
_LL747: _temp746=(( struct Cyc_Absyn_Let_d_struct*) _temp716)->f3; goto _LL745;
_LL745: _temp744=(( struct Cyc_Absyn_Let_d_struct*) _temp716)->f4; goto _LL743;
_LL743: _temp742=(( struct Cyc_Absyn_Let_d_struct*) _temp716)->f5; goto _LL719;}
else{ goto _LL720;} _LL720: if(*(( int*) _temp716) == Cyc_Absyn_Letv_d){ _LL753:
_temp752=(( struct Cyc_Absyn_Letv_d_struct*) _temp716)->f1; goto _LL721;} else{
goto _LL722;} _LL722: if(*(( int*) _temp716) == Cyc_Absyn_Var_d){ _LL755:
_temp754=(( struct Cyc_Absyn_Var_d_struct*) _temp716)->f1; goto _LL723;} else{
goto _LL724;} _LL724: if(*(( int*) _temp716) == Cyc_Absyn_Fn_d){ _LL757:
_temp756=(( struct Cyc_Absyn_Fn_d_struct*) _temp716)->f1; goto _LL725;} else{
goto _LL726;} _LL726: if(*(( int*) _temp716) == Cyc_Absyn_Typedef_d){ _LL759:
_temp758=(( struct Cyc_Absyn_Typedef_d_struct*) _temp716)->f1; goto _LL727;}
else{ goto _LL728;} _LL728: if(*(( int*) _temp716) == Cyc_Absyn_Struct_d){
_LL761: _temp760=(( struct Cyc_Absyn_Struct_d_struct*) _temp716)->f1; goto
_LL729;} else{ goto _LL730;} _LL730: if(*(( int*) _temp716) == Cyc_Absyn_Union_d){
_LL763: _temp762=(( struct Cyc_Absyn_Union_d_struct*) _temp716)->f1; goto _LL731;}
else{ goto _LL732;} _LL732: if(*(( int*) _temp716) == Cyc_Absyn_Tunion_d){
_LL765: _temp764=(( struct Cyc_Absyn_Tunion_d_struct*) _temp716)->f1; goto
_LL733;} else{ goto _LL734;} _LL734: if(*(( int*) _temp716) == Cyc_Absyn_Enum_d){
_LL767: _temp766=(( struct Cyc_Absyn_Enum_d_struct*) _temp716)->f1; goto _LL735;}
else{ goto _LL736;} _LL736: if(*(( int*) _temp716) == Cyc_Absyn_Namespace_d){
_LL771: _temp770=(( struct Cyc_Absyn_Namespace_d_struct*) _temp716)->f1; goto
_LL769; _LL769: _temp768=(( struct Cyc_Absyn_Namespace_d_struct*) _temp716)->f2;
goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp716) == Cyc_Absyn_Using_d){
_LL775: _temp774=(( struct Cyc_Absyn_Using_d_struct*) _temp716)->f1; _temp776=*
_temp774; _LL780: _temp779= _temp776.f1; goto _LL778; _LL778: _temp777= _temp776.f2;
goto _LL773; _LL773: _temp772=(( struct Cyc_Absyn_Using_d_struct*) _temp716)->f2;
goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp716) == Cyc_Absyn_ExternC_d){
_LL782: _temp781=(( struct Cyc_Absyn_ExternC_d_struct*) _temp716)->f1; goto
_LL741;} else{ goto _LL717;} _LL719: Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u)); goto _LL717; _LL721: Cyc_Tcutil_terr( loc,
_tag_arr("top level let-declarations are not implemented", sizeof( unsigned char),
47u)); goto _LL717; _LL723: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp754->sc): 0){( void*)( _temp754->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcVardecl( te, ge, loc, _temp754, check_var_init); goto _LL717; _LL725:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp756->sc): 0){( void*)(
_temp756->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcFndecl( te, ge,
loc, _temp756); goto _LL717; _LL727: Cyc_Tc_tcTypedefdecl( te, ge, loc, _temp758);
goto _LL717; _LL729: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp760->sc): 0){( void*)( _temp760->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp760); goto _LL717; _LL731: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp762->sc): 0){( void*)( _temp762->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp762);
goto _LL717; _LL733: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp764->sc): 0){( void*)( _temp764->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp764); goto _LL717; _LL735: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp766->sc): 0){( void*)( _temp766->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp766);
goto _LL717; _LL737: { struct Cyc_List_List* _temp783= te->ns; goto _LL784;
_LL784: { struct Cyc_List_List* _temp786=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp783,({ struct Cyc_List_List*
_temp785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp785->hd=( void*) _temp770; _temp785->tl= 0; _temp785;})); goto _LL787;
_LL787: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
ge->namespaces, _temp770)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_insert)( ge->namespaces, _temp770); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp786, Cyc_Tcenv_empty_genv());}
te->ns= _temp786; Cyc_Tc_tc_decls( te, _temp768, in_externC, check_var_init); te->ns=
_temp783; goto _LL717;}} _LL739: { struct _tagged_arr* first; struct Cyc_List_List*
rest;{ void* _temp788= _temp779; struct Cyc_List_List* _temp800; struct Cyc_List_List*
_temp802; struct Cyc_List_List* _temp804; struct Cyc_List_List _temp806; struct
Cyc_List_List* _temp807; struct _tagged_arr* _temp809; struct Cyc_List_List*
_temp811; struct Cyc_List_List _temp813; struct Cyc_List_List* _temp814; struct
_tagged_arr* _temp816; _LL790: if( _temp788 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL791;} else{ goto _LL792;} _LL792: if(( unsigned int) _temp788 > 1u?*(( int*)
_temp788) == Cyc_Absyn_Rel_n: 0){ _LL801: _temp800=(( struct Cyc_Absyn_Rel_n_struct*)
_temp788)->f1; if( _temp800 == 0){ goto _LL793;} else{ goto _LL794;}} else{ goto
_LL794;} _LL794: if(( unsigned int) _temp788 > 1u?*(( int*) _temp788) == Cyc_Absyn_Abs_n:
0){ _LL803: _temp802=(( struct Cyc_Absyn_Abs_n_struct*) _temp788)->f1; if(
_temp802 == 0){ goto _LL795;} else{ goto _LL796;}} else{ goto _LL796;} _LL796:
if(( unsigned int) _temp788 > 1u?*(( int*) _temp788) == Cyc_Absyn_Rel_n: 0){
_LL805: _temp804=(( struct Cyc_Absyn_Rel_n_struct*) _temp788)->f1; if( _temp804
== 0){ goto _LL798;} else{ _temp806=* _temp804; _LL810: _temp809=( struct
_tagged_arr*) _temp806.hd; goto _LL808; _LL808: _temp807= _temp806.tl; goto
_LL797;}} else{ goto _LL798;} _LL798: if(( unsigned int) _temp788 > 1u?*(( int*)
_temp788) == Cyc_Absyn_Abs_n: 0){ _LL812: _temp811=(( struct Cyc_Absyn_Abs_n_struct*)
_temp788)->f1; if( _temp811 == 0){ goto _LL789;} else{ _temp813=* _temp811;
_LL817: _temp816=( struct _tagged_arr*) _temp813.hd; goto _LL815; _LL815:
_temp814= _temp813.tl; goto _LL799;}} else{ goto _LL789;} _LL791: goto _LL793;
_LL793: goto _LL795; _LL795: first= _temp777; rest= 0; goto _LL789; _LL797:
_temp816= _temp809; _temp814= _temp807; goto _LL799; _LL799: first= _temp816;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp814,({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp818->hd=( void*) _temp777;
_temp818->tl= 0; _temp818;})); goto _LL789; _LL789:;}{ struct Cyc_List_List*
_temp819= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL820;
_LL820: ge->availables=({ struct Cyc_List_List* _temp821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp821->hd=( void*) _temp819;
_temp821->tl= ge->availables; _temp821;}); Cyc_Tc_tc_decls( te, _temp772,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL717;}} _LL741: Cyc_Tc_tc_decls( te,
_temp781, 1, check_var_init); goto _LL717; _LL717:;}} void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int check_var_init, struct Cyc_List_List* ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls( te, ds, 0, check_var_init);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d); static struct Cyc_List_List*
Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return((
struct Cyc_List_List*(*)( int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*),
struct Cyc_Dict_Dict* env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed,
env, ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; }
; static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp822=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp832; struct Cyc_List_List*
_temp834; struct Cyc_List_List** _temp836; struct _tuple0* _temp837; struct Cyc_List_List*
_temp839; struct Cyc_List_List** _temp841; struct _tagged_arr* _temp842; _LL824:
if(*(( int*) _temp822) == Cyc_Absyn_Var_d){ _LL833: _temp832=(( struct Cyc_Absyn_Var_d_struct*)
_temp822)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp822)
== Cyc_Absyn_Using_d){ _LL838: _temp837=(( struct Cyc_Absyn_Using_d_struct*)
_temp822)->f1; goto _LL835; _LL835: _temp834=(( struct Cyc_Absyn_Using_d_struct*)
_temp822)->f2; _temp836=&(( struct Cyc_Absyn_Using_d_struct*) _temp822)->f2;
goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp822) == Cyc_Absyn_Namespace_d){
_LL843: _temp842=(( struct Cyc_Absyn_Namespace_d_struct*) _temp822)->f1; goto
_LL840; _LL840: _temp839=(( struct Cyc_Absyn_Namespace_d_struct*) _temp822)->f2;
_temp841=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp822)->f2; goto _LL829;}
else{ goto _LL830;} _LL830: goto _LL831; _LL825: if(( void*) _temp832->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp846; struct
_tagged_arr* _temp847; void* _temp849; struct _tuple0* _temp844= _temp832->name;
_temp846=* _temp844; _LL850: _temp849= _temp846.f1; goto _LL848; _LL848:
_temp847= _temp846.f2; goto _LL845; _LL845: { struct Cyc_List_List* ns;{ void*
_temp851= _temp849; struct Cyc_List_List* _temp859; struct Cyc_List_List*
_temp861; _LL853: if( _temp851 ==( void*) Cyc_Absyn_Loc_n){ goto _LL854;} else{
goto _LL855;} _LL855: if(( unsigned int) _temp851 > 1u?*(( int*) _temp851) ==
Cyc_Absyn_Rel_n: 0){ _LL860: _temp859=(( struct Cyc_Absyn_Rel_n_struct*)
_temp851)->f1; goto _LL856;} else{ goto _LL857;} _LL857: if(( unsigned int)
_temp851 > 1u?*(( int*) _temp851) == Cyc_Absyn_Abs_n: 0){ _LL862: _temp861=((
struct Cyc_Absyn_Abs_n_struct*) _temp851)->f1; goto _LL858;} else{ goto _LL852;}
_LL854: ns= 0; goto _LL852; _LL856: ns= _temp859; goto _LL852; _LL858: ns=
_temp861; goto _LL852; _LL852:;}{ struct _tuple6* _temp863=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL864; _LL864: { struct Cyc_Tcenv_Genv* _temp865=(* _temp863).f1; goto
_LL866; _LL866: { int _temp867=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp865->ordinaries, _temp847)).f2;
goto _LL868; _LL868: if( ! _temp867){(* _temp863).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((* _temp863).f2,
_temp847);} return _temp867;}}}}} _LL827: _temp841= _temp836; goto _LL829;
_LL829:* _temp841= Cyc_Tc_treeshake_f( env,* _temp841); return 1; _LL831: return
1; _LL823:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp869=(* nsenv).f1; goto _LL870; _LL870: { struct Cyc_Set_Set* _temp871=(*
nsenv).f2; goto _LL872; _LL872: _temp869->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*, struct _tuple3*), struct Cyc_Set_Set*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp871, _temp869->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp873=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp873->f1= ge; _temp873->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_String_strptrcmp); _temp873;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp874=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL875; _LL875: { struct Cyc_List_List* _temp876= Cyc_Tc_treeshake_f(
_temp874, ds); goto _LL877; _LL877:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp874); return _temp876;}}