 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_InvalidArg[ 15u];
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
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_rcopy(
struct _RegionHandle*, struct Cyc_List_List* src); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_rempty( struct _RegionHandle*, int(*
comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void*
key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d,
void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict*,
void*); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_zstrptrcmp(
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple0*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc); struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_rempty( struct _RegionHandle* rgn, int(* comp)( void*, void*));
extern struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle* rgn, struct
Cyc_Set_Set* s, void* elt); extern struct Cyc_List_List* Cyc_Set_elements(
struct Cyc_Set_Set* s); extern unsigned char Cyc_Set_Absent[ 11u]; static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern struct _tuple0* Cyc_Toc_temp_var(); extern struct Cyc_List_List*
Cyc_Tovc_tovc( struct Cyc_List_List* decls); struct Cyc_Tovc_BoxingEnv{ struct
_RegionHandle* rgn; struct Cyc_List_List* all_locals; struct Cyc_Dict_Dict*
varmap; struct Cyc_List_List* boundvars; struct Cyc_Set_Set** freevars; } ;
struct Cyc_Tovc_ToExpEnv{ struct _RegionHandle* rgn; struct Cyc_List_List*
all_locals; struct Cyc_Dict_Dict* varmap; struct Cyc_Absyn_Stmt* encloser;
struct Cyc_List_List** gen_ds; } ; struct _tuple3{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Tovc_box_free_vars_exp( struct Cyc_Tovc_BoxingEnv
env, struct Cyc_Absyn_Exp* e){ while( 1) { void* _temp0=( void*) e->r; void*
_temp48; struct _tuple0* _temp50; struct _tuple0 _temp52; struct _tagged_arr*
_temp53; void* _temp55; void* _temp57; struct _tuple0* _temp59; void* _temp61;
struct Cyc_Absyn_VarargCallInfo* _temp63; struct Cyc_List_List* _temp65; struct
Cyc_Absyn_Exp* _temp67; struct Cyc_List_List* _temp69; void* _temp71; struct Cyc_Absyn_Exp*
_temp73; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp* _temp77; struct
Cyc_Absyn_Exp* _temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
Cyc_Core_Opt* _temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Exp*
_temp93; struct Cyc_List_List* _temp95; struct Cyc_Absyn_Exp* _temp97; struct
Cyc_Absyn_Exp* _temp99; void* _temp101; struct Cyc_Absyn_Exp* _temp103; struct
Cyc_Absyn_Exp* _temp105; struct Cyc_Absyn_Exp* _temp107; struct _tagged_arr*
_temp109; struct Cyc_Absyn_Exp* _temp111; struct _tagged_arr* _temp113; struct
Cyc_Absyn_Exp* _temp115; void* _temp117; struct Cyc_Absyn_Exp* _temp119; void*
_temp121; struct _tagged_arr* _temp123; void* _temp125; struct Cyc_Absyn_Enumfield*
_temp127; struct Cyc_Absyn_Enumdecl* _temp129; struct _tuple0* _temp131; struct
Cyc_List_List* _temp133; struct Cyc_Core_Opt* _temp135; _LL2: if(*(( int*)
_temp0) == Cyc_Absyn_Var_e){ _LL51: _temp50=(( struct Cyc_Absyn_Var_e_struct*)
_temp0)->f1; _temp52=* _temp50; _LL56: _temp55= _temp52.f1; if( _temp55 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL54;} else{ goto _LL4;} _LL54: _temp53= _temp52.f2;
goto _LL49; _LL49: _temp48=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp0)->f2;
goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) == Cyc_Absyn_Var_e){
_LL60: _temp59=(( struct Cyc_Absyn_Var_e_struct*) _temp0)->f1; goto _LL58; _LL58:
_temp57=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp0)->f2; goto _LL5;}
else{ goto _LL6;} _LL6: if(*(( int*) _temp0) == Cyc_Absyn_Const_e){ _LL62:
_temp61=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp0)->f1; goto _LL7;}
else{ goto _LL8;} _LL8: if(*(( int*) _temp0) == Cyc_Absyn_FnCall_e){ _LL68:
_temp67=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f1; goto _LL66; _LL66:
_temp65=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f2; goto _LL64; _LL64:
_temp63=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f3; goto _LL9;} else{
goto _LL10;} _LL10: if(*(( int*) _temp0) == Cyc_Absyn_Primop_e){ _LL72: _temp71=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f1; goto _LL70; _LL70:
_temp69=(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f2; goto _LL11;} else{
goto _LL12;} _LL12: if(*(( int*) _temp0) == Cyc_Absyn_Conditional_e){ _LL78:
_temp77=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f1; goto _LL76;
_LL76: _temp75=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f2; goto
_LL74; _LL74: _temp73=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f3;
goto _LL13;} else{ goto _LL14;} _LL14: if(*(( int*) _temp0) == Cyc_Absyn_Subscript_e){
_LL82: _temp81=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f1; goto _LL80;
_LL80: _temp79=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(*(( int*) _temp0) == Cyc_Absyn_SeqExp_e){ _LL86:
_temp85=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f1; goto _LL84; _LL84:
_temp83=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f2; goto _LL17;} else{
goto _LL18;} _LL18: if(*(( int*) _temp0) == Cyc_Absyn_AssignOp_e){ _LL92:
_temp91=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f1; goto _LL90; _LL90:
_temp89=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f2; goto _LL88; _LL88:
_temp87=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f3; goto _LL19;} else{
goto _LL20;} _LL20: if(*(( int*) _temp0) == Cyc_Absyn_NoInstantiate_e){ _LL94:
_temp93=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp0)->f1; goto _LL21;}
else{ goto _LL22;} _LL22: if(*(( int*) _temp0) == Cyc_Absyn_Instantiate_e){
_LL98: _temp97=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f1; goto
_LL96; _LL96: _temp95=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f2;
goto _LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp0) == Cyc_Absyn_Cast_e){
_LL102: _temp101=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f1; goto
_LL100; _LL100: _temp99=(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f2; goto
_LL25;} else{ goto _LL26;} _LL26: if(*(( int*) _temp0) == Cyc_Absyn_Address_e){
_LL104: _temp103=(( struct Cyc_Absyn_Address_e_struct*) _temp0)->f1; goto _LL27;}
else{ goto _LL28;} _LL28: if(*(( int*) _temp0) == Cyc_Absyn_Sizeofexp_e){ _LL106:
_temp105=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp0)->f1; goto _LL29;}
else{ goto _LL30;} _LL30: if(*(( int*) _temp0) == Cyc_Absyn_Deref_e){ _LL108:
_temp107=(( struct Cyc_Absyn_Deref_e_struct*) _temp0)->f1; goto _LL31;} else{
goto _LL32;} _LL32: if(*(( int*) _temp0) == Cyc_Absyn_StructMember_e){ _LL112:
_temp111=(( struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f1; goto _LL110;
_LL110: _temp109=(( struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f2; goto
_LL33;} else{ goto _LL34;} _LL34: if(*(( int*) _temp0) == Cyc_Absyn_StructArrow_e){
_LL116: _temp115=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f1; goto
_LL114; _LL114: _temp113=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f2;
goto _LL35;} else{ goto _LL36;} _LL36: if(*(( int*) _temp0) == Cyc_Absyn_Increment_e){
_LL120: _temp119=(( struct Cyc_Absyn_Increment_e_struct*) _temp0)->f1; goto
_LL118; _LL118: _temp117=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp0)->f2;
goto _LL37;} else{ goto _LL38;} _LL38: if(*(( int*) _temp0) == Cyc_Absyn_Sizeoftyp_e){
_LL122: _temp121=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp0)->f1;
goto _LL39;} else{ goto _LL40;} _LL40: if(*(( int*) _temp0) == Cyc_Absyn_Offsetof_e){
_LL126: _temp125=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp0)->f1;
goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp0)->f2;
goto _LL41;} else{ goto _LL42;} _LL42: if(*(( int*) _temp0) == Cyc_Absyn_Enum_e){
_LL132: _temp131=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f1; goto _LL130;
_LL130: _temp129=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f2; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f3; goto _LL43;}
else{ goto _LL44;} _LL44: if(*(( int*) _temp0) == Cyc_Absyn_UnresolvedMem_e){
_LL136: _temp135=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f1; goto
_LL134; _LL134: _temp133=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f2;
goto _LL45;} else{ goto _LL46;} _LL46: goto _LL47; _LL3: if((( int(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)( env.varmap,
_temp53)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( env.varmap,
_temp53))->r)); continue;} for( 0; env.boundvars != 0; env.boundvars=(( struct
Cyc_List_List*) _check_null( env.boundvars))->tl){ if( Cyc_String_zstrptrcmp(
_temp53,(*(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
env.boundvars))->hd)->name).f2) == 0){ return;}} for( 0; env.all_locals != 0;
env.all_locals=(( struct Cyc_List_List*) _check_null( env.all_locals))->tl){
struct Cyc_Absyn_Vardecl* _temp137=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( env.all_locals))->hd; goto _LL138; _LL138: if( Cyc_String_zstrptrcmp(
_temp53,(* _temp137->name).f2) == 0){{ void* _temp139=( void*) _temp137->type;
struct Cyc_Absyn_Exp* _temp145; struct Cyc_Absyn_Tqual _temp147; void* _temp149;
_LL141: if(( unsigned int) _temp139 > 4u?*(( int*) _temp139) == Cyc_Absyn_ArrayType:
0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp139)->f1;
goto _LL148; _LL148: _temp147=(( struct Cyc_Absyn_ArrayType_struct*) _temp139)->f2;
goto _LL146; _LL146: _temp145=(( struct Cyc_Absyn_ArrayType_struct*) _temp139)->f3;
goto _LL142;} else{ goto _LL143;} _LL143: goto _LL144; _LL142: goto _LL140;
_LL144:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp137->name, 0), 0))->r)); goto _LL140; _LL140:;}* env.freevars=(( struct Cyc_Set_Set*(*)(
struct _RegionHandle* rgn, struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt))
Cyc_Set_rinsert)( env.rgn,* env.freevars, _temp137); return;}}( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp151=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp151[ 0]=({ struct
Cyc_Core_Impossible_struct _temp152; _temp152.tag= Cyc_Core_Impossible; _temp152.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp153=* _temp53; xprintf("unbound variable %.*s found in box_free_vars_exp",
_get_arr_size( _temp153, 1u), _temp153.curr);}); _temp152;}); _temp151;})); _LL5:
return; _LL7: return; _LL9: Cyc_Tovc_box_free_vars_exp( env, _temp67); _temp69=
_temp65; goto _LL11; _LL11: for( 0; _temp69 != 0; _temp69=(( struct Cyc_List_List*)
_check_null( _temp69))->tl){ Cyc_Tovc_box_free_vars_exp( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp69))->hd);} return; _LL13: Cyc_Tovc_box_free_vars_exp(
env, _temp73); _temp81= _temp77; _temp79= _temp75; goto _LL15; _LL15: _temp85=
_temp81; _temp83= _temp79; goto _LL17; _LL17: _temp91= _temp85; _temp87= _temp83;
goto _LL19; _LL19: Cyc_Tovc_box_free_vars_exp( env, _temp87); _temp93= _temp91;
goto _LL21; _LL21: _temp97= _temp93; goto _LL23; _LL23: _temp99= _temp97; goto
_LL25; _LL25: _temp103= _temp99; goto _LL27; _LL27: _temp105= _temp103; goto
_LL29; _LL29: _temp107= _temp105; goto _LL31; _LL31: _temp111= _temp107; goto
_LL33; _LL33: _temp115= _temp111; goto _LL35; _LL35: _temp119= _temp115; goto
_LL37; _LL37: Cyc_Tovc_box_free_vars_exp( env, _temp119); return; _LL39: return;
_LL41: return; _LL43: return; _LL45: for( 0; _temp133 != 0; _temp133=(( struct
Cyc_List_List*) _check_null( _temp133))->tl){ Cyc_Tovc_box_free_vars_exp( env,(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp133))->hd)).f2);}
return; _LL47:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp154=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp154[ 0]=({ struct Cyc_Core_Impossible_struct _temp155; _temp155.tag= Cyc_Core_Impossible;
_temp155.f1= _tag_arr("bad exp form in Tocv::box_free_vars_exp", sizeof(
unsigned char), 40u); _temp155;}); _temp154;})); _LL1:;}} static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { void*
_temp156=( void*) s->r; struct Cyc_Absyn_Stmt* _temp188; struct Cyc_Absyn_Stmt*
_temp190; struct Cyc_Absyn_Stmt* _temp192; struct _tagged_arr* _temp194; struct
Cyc_Absyn_Stmt* _temp196; struct _tagged_arr* _temp198; struct Cyc_Absyn_Exp*
_temp200; struct Cyc_Absyn_Exp* _temp202; struct Cyc_Absyn_Stmt* _temp204;
struct Cyc_Absyn_Stmt* _temp206; struct Cyc_Absyn_Stmt* _temp208; struct Cyc_Absyn_Stmt*
_temp210; struct Cyc_Absyn_Exp* _temp212; struct Cyc_Absyn_Stmt* _temp214;
struct _tuple2 _temp216; struct Cyc_Absyn_Stmt* _temp218; struct Cyc_Absyn_Exp*
_temp220; struct Cyc_Absyn_Stmt* _temp222; struct _tuple2 _temp224; struct Cyc_Absyn_Stmt*
_temp226; struct Cyc_Absyn_Exp* _temp228; struct _tuple2 _temp230; struct Cyc_Absyn_Stmt*
_temp232; struct Cyc_Absyn_Exp* _temp234; struct Cyc_Absyn_Exp* _temp236; struct
_tuple2 _temp238; struct Cyc_Absyn_Stmt* _temp240; struct Cyc_Absyn_Exp*
_temp242; struct Cyc_Absyn_Stmt* _temp244; struct Cyc_List_List* _temp246;
struct Cyc_Absyn_Exp* _temp248; struct Cyc_Absyn_Stmt* _temp250; struct Cyc_Absyn_Decl*
_temp252; _LL158: if( _temp156 ==( void*) Cyc_Absyn_Skip_s){ goto _LL159;} else{
goto _LL160;} _LL160: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) ==
Cyc_Absyn_Break_s: 0){ _LL189: _temp188=(( struct Cyc_Absyn_Break_s_struct*)
_temp156)->f1; goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int)
_temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Continue_s: 0){ _LL191: _temp190=((
struct Cyc_Absyn_Continue_s_struct*) _temp156)->f1; goto _LL163;} else{ goto
_LL164;} _LL164: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Goto_s:
0){ _LL195: _temp194=(( struct Cyc_Absyn_Goto_s_struct*) _temp156)->f1; goto
_LL193; _LL193: _temp192=(( struct Cyc_Absyn_Goto_s_struct*) _temp156)->f2; goto
_LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp156 > 1u?*(( int*)
_temp156) == Cyc_Absyn_Label_s: 0){ _LL199: _temp198=(( struct Cyc_Absyn_Label_s_struct*)
_temp156)->f1; goto _LL197; _LL197: _temp196=(( struct Cyc_Absyn_Label_s_struct*)
_temp156)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Return_s: 0){ _LL201: _temp200=((
struct Cyc_Absyn_Return_s_struct*) _temp156)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Exp_s:
0){ _LL203: _temp202=(( struct Cyc_Absyn_Exp_s_struct*) _temp156)->f1; goto
_LL171;} else{ goto _LL172;} _LL172: if(( unsigned int) _temp156 > 1u?*(( int*)
_temp156) == Cyc_Absyn_Seq_s: 0){ _LL207: _temp206=(( struct Cyc_Absyn_Seq_s_struct*)
_temp156)->f1; goto _LL205; _LL205: _temp204=(( struct Cyc_Absyn_Seq_s_struct*)
_temp156)->f2; goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_IfThenElse_s: 0){ _LL213:
_temp212=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp156)->f1; goto _LL211;
_LL211: _temp210=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp156)->f2; goto
_LL209; _LL209: _temp208=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp156)->f3;
goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int) _temp156 > 1u?*((
int*) _temp156) == Cyc_Absyn_While_s: 0){ _LL217: _temp216=(( struct Cyc_Absyn_While_s_struct*)
_temp156)->f1; _LL221: _temp220= _temp216.f1; goto _LL219; _LL219: _temp218=
_temp216.f2; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_While_s_struct*)
_temp156)->f2; goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int)
_temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_For_s: 0){ _LL237: _temp236=((
struct Cyc_Absyn_For_s_struct*) _temp156)->f1; goto _LL231; _LL231: _temp230=((
struct Cyc_Absyn_For_s_struct*) _temp156)->f2; _LL235: _temp234= _temp230.f1;
goto _LL233; _LL233: _temp232= _temp230.f2; goto _LL225; _LL225: _temp224=((
struct Cyc_Absyn_For_s_struct*) _temp156)->f3; _LL229: _temp228= _temp224.f1;
goto _LL227; _LL227: _temp226= _temp224.f2; goto _LL223; _LL223: _temp222=((
struct Cyc_Absyn_For_s_struct*) _temp156)->f4; goto _LL179;} else{ goto _LL180;}
_LL180: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Do_s:
0){ _LL245: _temp244=(( struct Cyc_Absyn_Do_s_struct*) _temp156)->f1; goto
_LL239; _LL239: _temp238=(( struct Cyc_Absyn_Do_s_struct*) _temp156)->f2; _LL243:
_temp242= _temp238.f1; goto _LL241; _LL241: _temp240= _temp238.f2; goto _LL181;}
else{ goto _LL182;} _LL182: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156)
== Cyc_Absyn_Switch_s: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Switch_s_struct*)
_temp156)->f1; goto _LL247; _LL247: _temp246=(( struct Cyc_Absyn_Switch_s_struct*)
_temp156)->f2; goto _LL183;} else{ goto _LL184;} _LL184: if(( unsigned int)
_temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Decl_s: 0){ _LL253: _temp252=((
struct Cyc_Absyn_Decl_s_struct*) _temp156)->f1; goto _LL251; _LL251: _temp250=((
struct Cyc_Absyn_Decl_s_struct*) _temp156)->f2; goto _LL185;} else{ goto _LL186;}
_LL186: goto _LL187; _LL159: return; _LL161: return; _LL163: return; _LL165:
return; _LL167: s= _temp196; continue; _LL169: if( _temp200 == 0){ return;}
_temp202=( struct Cyc_Absyn_Exp*) _check_null( _temp200); goto _LL171; _LL171:
Cyc_Tovc_box_free_vars_exp( env, _temp202); return; _LL173: Cyc_Tovc_box_free_vars_stmt(
env, _temp206); s= _temp204; continue; _LL175: Cyc_Tovc_box_free_vars_exp( env,
_temp212); Cyc_Tovc_box_free_vars_stmt( env, _temp210); s= _temp208; continue;
_LL177: Cyc_Tovc_box_free_vars_exp( env, _temp220); s= _temp214; continue;
_LL179: Cyc_Tovc_box_free_vars_exp( env, _temp236); Cyc_Tovc_box_free_vars_exp(
env, _temp234); Cyc_Tovc_box_free_vars_exp( env, _temp228); s= _temp222;
continue; _LL181: Cyc_Tovc_box_free_vars_exp( env, _temp242); s= _temp244;
continue; _LL183: Cyc_Tovc_box_free_vars_exp( env, _temp248); for( 0; _temp246
!= 0; _temp246=(( struct Cyc_List_List*) _check_null( _temp246))->tl){ Cyc_Tovc_box_free_vars_stmt(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp246))->hd)->body);} return; _LL185:{ void* _temp254=( void*) _temp252->r;
struct Cyc_Absyn_Vardecl* _temp260; _LL256: if(*(( int*) _temp254) == Cyc_Absyn_Var_d){
_LL261: _temp260=(( struct Cyc_Absyn_Var_d_struct*) _temp254)->f1; goto _LL257;}
else{ goto _LL258;} _LL258: goto _LL259; _LL257: env.boundvars=({ struct Cyc_List_List*
_temp262=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp262->hd=( void*) _temp260; _temp262->tl= env.boundvars; _temp262;}); env.varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*, struct _tagged_arr*)) Cyc_Dict_rdelete_same)(
env.varmap,(* _temp260->name).f2); if( _temp260->initializer != 0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp260->initializer));} s= _temp250;
continue; _LL259: goto _LL255; _LL255:;} goto _LL187; _LL187:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp263=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp263[ 0]=({ struct
Cyc_Core_Impossible_struct _temp264; _temp264.tag= Cyc_Core_Impossible; _temp264.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp264;});
_temp263;})); _LL157:;}} struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* rgn, int(* comp)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*))) Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp266=({
struct Cyc_Tovc_BoxingEnv _temp265; _temp265.rgn= env.rgn; _temp265.all_locals=
env.all_locals; _temp265.varmap= env.varmap; _temp265.boundvars= 0; _temp265.freevars=&
freevars; _temp265;}); goto _LL267; _LL267: Cyc_Tovc_box_free_vars_stmt(
_temp266, s);{ struct Cyc_List_List* params= 0; struct Cyc_List_List* args= 0;
struct Cyc_List_List* free_vars=(( struct Cyc_List_List*(*)( struct Cyc_Set_Set*
s)) Cyc_Set_elements)( freevars); for( 0; free_vars != 0; free_vars=(( struct
Cyc_List_List*) _check_null( free_vars))->tl){ struct Cyc_Absyn_Vardecl* vd=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( free_vars))->hd;
void* _temp268=( void*) vd->type; struct Cyc_Absyn_Exp* _temp274; struct Cyc_Absyn_Tqual
_temp276; void* _temp278; _LL270: if(( unsigned int) _temp268 > 4u?*(( int*)
_temp268) == Cyc_Absyn_ArrayType: 0){ _LL279: _temp278=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp268)->f1; goto _LL277; _LL277: _temp276=(( struct Cyc_Absyn_ArrayType_struct*)
_temp268)->f2; goto _LL275; _LL275: _temp274=(( struct Cyc_Absyn_ArrayType_struct*)
_temp268)->f3; goto _LL271;} else{ goto _LL272;} _LL272: goto _LL273; _LL271:
params=({ struct Cyc_List_List* _temp280=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp280->hd=( void*)({ struct _tuple4* _temp281=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp281->f1=(* vd->name).f2;
_temp281->f2= vd->tq; _temp281->f3= Cyc_Absyn_cstar_typ( _temp278, Cyc_Absyn_empty_tqual());
_temp281;}); _temp280->tl= params; _temp280;}); args=({ struct Cyc_List_List*
_temp282=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp282->hd=( void*) Cyc_Absyn_var_exp( vd->name, 0); _temp282->tl= args;
_temp282;}); goto _LL269; _LL273: params=({ struct Cyc_List_List* _temp283=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp283->hd=(
void*)({ struct _tuple4* _temp284=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp284->f1=(* vd->name).f2; _temp284->f2= vd->tq; _temp284->f3= Cyc_Absyn_cstar_typ((
void*) vd->type, Cyc_Absyn_empty_tqual()); _temp284;}); _temp283->tl= params;
_temp283;}); args=({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp285->hd=( void*) Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp( vd->name, 0), 0); _temp285->tl= args; _temp285;}); goto
_LL269; _LL269:;}{ struct _tuple0* funname= Cyc_Toc_temp_var();* env.gen_ds=({
struct Cyc_List_List* _temp286=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp286->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp287=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp287[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp288; _temp288.tag= Cyc_Absyn_Fn_d; _temp288.f1=({ struct Cyc_Absyn_Fndecl*
_temp289=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp289->sc=( void*)(( void*) Cyc_Absyn_Static); _temp289->is_inline= 1;
_temp289->name= funname; _temp289->tvs= 0; _temp289->effect= 0; _temp289->ret_type=(
void*) rettype; _temp289->args= params; _temp289->c_varargs= 0; _temp289->cyc_varargs=
0; _temp289->rgn_po= 0; _temp289->body= s; _temp289->cached_typ= 0; _temp289->param_vardecls=
0; _temp289->attributes= 0; _temp289;}); _temp288;}); _temp287;}), 0); _temp286->tl=*
env.gen_ds; _temp286;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp290=( void*) e->r; void* _temp338; struct
_tuple0* _temp340; struct _tuple0 _temp342; struct _tagged_arr* _temp343; void*
_temp345; void* _temp347; struct _tuple0* _temp349; void* _temp351; struct Cyc_Absyn_VarargCallInfo*
_temp353; struct Cyc_List_List* _temp355; struct Cyc_Absyn_Exp* _temp357; struct
Cyc_List_List* _temp359; void* _temp361; struct Cyc_Absyn_Exp* _temp363; struct
Cyc_Absyn_Exp* _temp365; struct Cyc_Absyn_Exp* _temp367; struct Cyc_Absyn_Exp*
_temp369; struct Cyc_Absyn_Exp* _temp371; struct Cyc_Absyn_Exp* _temp373; struct
Cyc_Absyn_Exp* _temp375; struct Cyc_Absyn_Exp* _temp377; struct Cyc_Core_Opt*
_temp379; struct Cyc_Absyn_Exp* _temp381; struct Cyc_Absyn_Exp* _temp383; struct
Cyc_List_List* _temp385; struct Cyc_Absyn_Exp* _temp387; struct Cyc_Absyn_Exp*
_temp389; void* _temp391; struct Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Exp*
_temp395; struct Cyc_Absyn_Exp* _temp397; struct _tagged_arr* _temp399; struct
Cyc_Absyn_Exp* _temp401; struct _tagged_arr* _temp403; struct Cyc_Absyn_Exp*
_temp405; void* _temp407; struct Cyc_Absyn_Exp* _temp409; void* _temp411; struct
_tagged_arr* _temp413; void* _temp415; struct Cyc_Absyn_Enumfield* _temp417;
struct Cyc_Absyn_Enumdecl* _temp419; struct _tuple0* _temp421; struct Cyc_List_List*
_temp423; struct Cyc_Core_Opt* _temp425; _LL292: if(*(( int*) _temp290) == Cyc_Absyn_Var_e){
_LL341: _temp340=(( struct Cyc_Absyn_Var_e_struct*) _temp290)->f1; _temp342=*
_temp340; _LL346: _temp345= _temp342.f1; if( _temp345 ==( void*) Cyc_Absyn_Loc_n){
goto _LL344;} else{ goto _LL294;} _LL344: _temp343= _temp342.f2; goto _LL339;
_LL339: _temp338=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp290)->f2; goto
_LL293;} else{ goto _LL294;} _LL294: if(*(( int*) _temp290) == Cyc_Absyn_Var_e){
_LL350: _temp349=(( struct Cyc_Absyn_Var_e_struct*) _temp290)->f1; goto _LL348;
_LL348: _temp347=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp290)->f2; goto
_LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp290) == Cyc_Absyn_Const_e){
_LL352: _temp351=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp290)->f1;
goto _LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp290) == Cyc_Absyn_FnCall_e){
_LL358: _temp357=(( struct Cyc_Absyn_FnCall_e_struct*) _temp290)->f1; goto
_LL356; _LL356: _temp355=(( struct Cyc_Absyn_FnCall_e_struct*) _temp290)->f2;
goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_FnCall_e_struct*) _temp290)->f3;
goto _LL299;} else{ goto _LL300;} _LL300: if(*(( int*) _temp290) == Cyc_Absyn_Primop_e){
_LL362: _temp361=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp290)->f1;
goto _LL360; _LL360: _temp359=(( struct Cyc_Absyn_Primop_e_struct*) _temp290)->f2;
goto _LL301;} else{ goto _LL302;} _LL302: if(*(( int*) _temp290) == Cyc_Absyn_Conditional_e){
_LL368: _temp367=(( struct Cyc_Absyn_Conditional_e_struct*) _temp290)->f1; goto
_LL366; _LL366: _temp365=(( struct Cyc_Absyn_Conditional_e_struct*) _temp290)->f2;
goto _LL364; _LL364: _temp363=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp290)->f3; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*) _temp290)
== Cyc_Absyn_Subscript_e){ _LL372: _temp371=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp290)->f1; goto _LL370; _LL370: _temp369=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp290)->f2; goto _LL305;} else{ goto _LL306;} _LL306: if(*(( int*) _temp290)
== Cyc_Absyn_SeqExp_e){ _LL376: _temp375=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp290)->f1; goto _LL374; _LL374: _temp373=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp290)->f2; goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp290)
== Cyc_Absyn_AssignOp_e){ _LL382: _temp381=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp290)->f1; goto _LL380; _LL380: _temp379=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp290)->f2; goto _LL378; _LL378: _temp377=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp290)->f3; goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp290)
== Cyc_Absyn_NoInstantiate_e){ _LL384: _temp383=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp290)->f1; goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp290)
== Cyc_Absyn_Instantiate_e){ _LL388: _temp387=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp290)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp290)->f2; goto _LL313;} else{ goto _LL314;} _LL314: if(*(( int*) _temp290)
== Cyc_Absyn_Cast_e){ _LL392: _temp391=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp290)->f1; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_Cast_e_struct*)
_temp290)->f2; goto _LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp290)
== Cyc_Absyn_Address_e){ _LL394: _temp393=(( struct Cyc_Absyn_Address_e_struct*)
_temp290)->f1; goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp290)
== Cyc_Absyn_Sizeofexp_e){ _LL396: _temp395=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp290)->f1; goto _LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp290)
== Cyc_Absyn_Deref_e){ _LL398: _temp397=(( struct Cyc_Absyn_Deref_e_struct*)
_temp290)->f1; goto _LL321;} else{ goto _LL322;} _LL322: if(*(( int*) _temp290)
== Cyc_Absyn_StructMember_e){ _LL402: _temp401=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp290)->f1; goto _LL400; _LL400: _temp399=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp290)->f2; goto _LL323;} else{ goto _LL324;} _LL324: if(*(( int*) _temp290)
== Cyc_Absyn_StructArrow_e){ _LL406: _temp405=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp290)->f1; goto _LL404; _LL404: _temp403=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp290)->f2; goto _LL325;} else{ goto _LL326;} _LL326: if(*(( int*) _temp290)
== Cyc_Absyn_Increment_e){ _LL410: _temp409=(( struct Cyc_Absyn_Increment_e_struct*)
_temp290)->f1; goto _LL408; _LL408: _temp407=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp290)->f2; goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp290)
== Cyc_Absyn_Sizeoftyp_e){ _LL412: _temp411=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp290)->f1; goto _LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp290)
== Cyc_Absyn_Offsetof_e){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp290)->f1; goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp290)->f2; goto _LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp290)
== Cyc_Absyn_Enum_e){ _LL422: _temp421=(( struct Cyc_Absyn_Enum_e_struct*)
_temp290)->f1; goto _LL420; _LL420: _temp419=(( struct Cyc_Absyn_Enum_e_struct*)
_temp290)->f2; goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_Enum_e_struct*)
_temp290)->f3; goto _LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp290)
== Cyc_Absyn_UnresolvedMem_e){ _LL426: _temp425=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp290)->f1; goto _LL424; _LL424: _temp423=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp290)->f2; goto _LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL293:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
varmap, _temp343)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( varmap,
_temp343))->r));} return; _LL295: return; _LL297: return; _LL299: Cyc_Tovc_apply_varmap(
varmap, _temp357); _temp359= _temp355; goto _LL301; _LL301: for( 0; _temp359 !=
0; _temp359=(( struct Cyc_List_List*) _check_null( _temp359))->tl){ Cyc_Tovc_apply_varmap(
varmap,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp359))->hd);}
return; _LL303: Cyc_Tovc_apply_varmap( varmap, _temp363); _temp371= _temp367;
_temp369= _temp365; goto _LL305; _LL305: _temp375= _temp371; _temp373= _temp369;
goto _LL307; _LL307: _temp381= _temp375; _temp377= _temp373; goto _LL309; _LL309:
Cyc_Tovc_apply_varmap( varmap, _temp377); _temp383= _temp381; goto _LL311;
_LL311: _temp387= _temp383; goto _LL313; _LL313: _temp389= _temp387; goto _LL315;
_LL315: _temp393= _temp389; goto _LL317; _LL317: _temp395= _temp393; goto _LL319;
_LL319: _temp397= _temp395; goto _LL321; _LL321: _temp401= _temp397; goto _LL323;
_LL323: _temp405= _temp401; goto _LL325; _LL325: _temp409= _temp405; goto _LL327;
_LL327: Cyc_Tovc_apply_varmap( varmap, _temp409); return; _LL329: return; _LL331:
return; _LL333: return; _LL335: for( 0; _temp423 != 0; _temp423=(( struct Cyc_List_List*)
_check_null( _temp423))->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp423))->hd)).f2);} return; _LL337:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp427=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp427[ 0]=({ struct
Cyc_Core_Impossible_struct _temp428; _temp428.tag= Cyc_Core_Impossible; _temp428.f1=
_tag_arr("bad exp form in Tocv::apply_varmap", sizeof( unsigned char), 35u);
_temp428;}); _temp427;})); _LL291:;} static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_exp(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){ void* _temp429=( void*)
s->r; struct Cyc_Absyn_Exp* _temp443; struct Cyc_Absyn_Stmt* _temp445; struct
Cyc_Absyn_Stmt* _temp447; struct Cyc_Absyn_Stmt* _temp449; struct Cyc_Absyn_Stmt*
_temp451; struct Cyc_Absyn_Exp* _temp453; struct Cyc_Absyn_Stmt* _temp455;
struct Cyc_Absyn_Decl* _temp457; _LL431: if( _temp429 ==( void*) Cyc_Absyn_Skip_s){
goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int) _temp429 > 1u?*((
int*) _temp429) == Cyc_Absyn_Exp_s: 0){ _LL444: _temp443=(( struct Cyc_Absyn_Exp_s_struct*)
_temp429)->f1; goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int)
_temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Seq_s: 0){ _LL448: _temp447=((
struct Cyc_Absyn_Seq_s_struct*) _temp429)->f1; goto _LL446; _LL446: _temp445=((
struct Cyc_Absyn_Seq_s_struct*) _temp429)->f2; goto _LL436;} else{ goto _LL437;}
_LL437: if(( unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_IfThenElse_s:
0){ _LL454: _temp453=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp429)->f1;
goto _LL452; _LL452: _temp451=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp429)->f2;
goto _LL450; _LL450: _temp449=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp429)->f3;
goto _LL438;} else{ goto _LL439;} _LL439: if(( unsigned int) _temp429 > 1u?*((
int*) _temp429) == Cyc_Absyn_Decl_s: 0){ _LL458: _temp457=(( struct Cyc_Absyn_Decl_s_struct*)
_temp429)->f1; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_Decl_s_struct*)
_temp429)->f2; goto _LL440;} else{ goto _LL441;} _LL441: goto _LL442; _LL432:
return Cyc_Absyn_true_exp( 0); _LL434: Cyc_Tovc_apply_varmap( env.varmap,
_temp443); return _temp443; _LL436: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp447), Cyc_Tovc_stmt_to_exp( env, _temp445), 0); _LL438: Cyc_Tovc_apply_varmap(
env.varmap, _temp453); return Cyc_Absyn_conditional_exp( _temp453, Cyc_Tovc_stmt_to_exp(
env, _temp451), Cyc_Tovc_stmt_to_exp( env, _temp449), 0); _LL440: { void*
_temp459=( void*) _temp457->r; struct Cyc_Absyn_Vardecl* _temp465; _LL461: if(*((
int*) _temp459) == Cyc_Absyn_Var_d){ _LL466: _temp465=(( struct Cyc_Absyn_Var_d_struct*)
_temp459)->f1; goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462: {
struct _tagged_arr* _temp467=(* _temp465->name).f2; goto _LL468; _LL468: {
struct _tuple0* _temp469= Cyc_Toc_temp_var(); goto _LL470; _LL470: _temp465->name=
_temp469; env.varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( env.varmap,
_temp467, Cyc_Absyn_var_exp( _temp469, 0)); if( _temp465->initializer != 0){ Cyc_Tovc_apply_varmap(
env.varmap,( struct Cyc_Absyn_Exp*) _check_null( _temp465->initializer));{ void*
_temp471=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp465->initializer))->r;
struct Cyc_List_List* _temp477; struct Cyc_Core_Opt* _temp479; _LL473: if(*((
int*) _temp471) == Cyc_Absyn_UnresolvedMem_e){ _LL480: _temp479=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp471)->f1; goto _LL478; _LL478: _temp477=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp471)->f2; goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL474: {
struct _tuple0* tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp465->type, _temp465->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp465->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp465->type);
goto _LL472;} _LL476: goto _LL472; _LL472:;}} env.all_locals=({ struct Cyc_List_List*
_temp481=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp481->hd=( void*) _temp465; _temp481->tl= env.all_locals; _temp481;});{
struct Cyc_Absyn_Exp* _temp482= _temp465->initializer; goto _LL483; _LL483:
_temp465->initializer= 0;( void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp457, Cyc_Absyn_new_stmt(( void*)( env.encloser)->r, 0), 0))->r));{ struct
Cyc_Absyn_Exp* _temp484= Cyc_Tovc_stmt_to_exp( env, _temp455); goto _LL485;
_LL485: if( _temp482 != 0){ _temp484= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp465->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp482), 0), _temp484, 0);} return _temp484;}}}} _LL464:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp486=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp486[ 0]=({ struct
Cyc_Core_Impossible_struct _temp487; _temp487.tag= Cyc_Core_Impossible; _temp487.f1=
_tag_arr("bad local decl in Tovc::stmt_to_exp", sizeof( unsigned char), 36u);
_temp487;}); _temp486;})); _LL460:;} _LL442: return Cyc_Tovc_stmt_to_fun( env, s,(
void*) Cyc_Absyn_VoidType); _LL430:;} static void Cyc_Tovc_stmt_to_vc( struct
Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s); static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Exp* e){ void* _temp488=( void*)
e->r; void* _temp536; void* _temp538; struct _tuple0* _temp540; struct Cyc_Absyn_VarargCallInfo*
_temp542; struct Cyc_List_List* _temp544; struct Cyc_Absyn_Exp* _temp546; struct
Cyc_List_List* _temp548; void* _temp550; struct Cyc_Absyn_Exp* _temp552; struct
Cyc_Absyn_Exp* _temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Absyn_Exp*
_temp558; struct Cyc_Absyn_Exp* _temp560; struct Cyc_Absyn_Exp* _temp562; struct
Cyc_Absyn_Exp* _temp564; struct Cyc_Absyn_Exp* _temp566; struct Cyc_Core_Opt*
_temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp* _temp572; struct
Cyc_List_List* _temp574; struct Cyc_Absyn_Exp* _temp576; struct Cyc_Absyn_Exp*
_temp578; void* _temp580; struct Cyc_Absyn_Exp* _temp582; struct Cyc_Absyn_Exp*
_temp584; struct Cyc_Absyn_Exp* _temp586; struct _tagged_arr* _temp588; struct
Cyc_Absyn_Exp* _temp590; struct _tagged_arr* _temp592; struct Cyc_Absyn_Exp*
_temp594; void* _temp596; struct Cyc_Absyn_Exp* _temp598; void* _temp600; struct
_tagged_arr* _temp602; void* _temp604; struct Cyc_Absyn_Enumfield* _temp606;
struct Cyc_Absyn_Enumdecl* _temp608; struct _tuple0* _temp610; struct Cyc_List_List*
_temp612; struct Cyc_Core_Opt* _temp614; struct Cyc_Absyn_Stmt* _temp616; _LL490:
if(*(( int*) _temp488) == Cyc_Absyn_Const_e){ _LL537: _temp536=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp488)->f1; goto _LL491;} else{ goto _LL492;}
_LL492: if(*(( int*) _temp488) == Cyc_Absyn_Var_e){ _LL541: _temp540=(( struct
Cyc_Absyn_Var_e_struct*) _temp488)->f1; goto _LL539; _LL539: _temp538=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp488)->f2; goto _LL493;} else{ goto _LL494;}
_LL494: if(*(( int*) _temp488) == Cyc_Absyn_FnCall_e){ _LL547: _temp546=((
struct Cyc_Absyn_FnCall_e_struct*) _temp488)->f1; goto _LL545; _LL545: _temp544=((
struct Cyc_Absyn_FnCall_e_struct*) _temp488)->f2; goto _LL543; _LL543: _temp542=((
struct Cyc_Absyn_FnCall_e_struct*) _temp488)->f3; goto _LL495;} else{ goto
_LL496;} _LL496: if(*(( int*) _temp488) == Cyc_Absyn_Primop_e){ _LL551: _temp550=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp488)->f1; goto _LL549; _LL549:
_temp548=(( struct Cyc_Absyn_Primop_e_struct*) _temp488)->f2; goto _LL497;}
else{ goto _LL498;} _LL498: if(*(( int*) _temp488) == Cyc_Absyn_Conditional_e){
_LL557: _temp556=(( struct Cyc_Absyn_Conditional_e_struct*) _temp488)->f1; goto
_LL555; _LL555: _temp554=(( struct Cyc_Absyn_Conditional_e_struct*) _temp488)->f2;
goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp488)->f3; goto _LL499;} else{ goto _LL500;} _LL500: if(*(( int*) _temp488)
== Cyc_Absyn_Subscript_e){ _LL561: _temp560=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp488)->f1; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp488)->f2; goto _LL501;} else{ goto _LL502;} _LL502: if(*(( int*) _temp488)
== Cyc_Absyn_SeqExp_e){ _LL565: _temp564=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp488)->f1; goto _LL563; _LL563: _temp562=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp488)->f2; goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp488)
== Cyc_Absyn_AssignOp_e){ _LL571: _temp570=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp488)->f1; goto _LL569; _LL569: _temp568=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp488)->f2; goto _LL567; _LL567: _temp566=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp488)->f3; goto _LL505;} else{ goto _LL506;} _LL506: if(*(( int*) _temp488)
== Cyc_Absyn_NoInstantiate_e){ _LL573: _temp572=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp488)->f1; goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp488)
== Cyc_Absyn_Instantiate_e){ _LL577: _temp576=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp488)->f1; goto _LL575; _LL575: _temp574=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp488)->f2; goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp488)
== Cyc_Absyn_Cast_e){ _LL581: _temp580=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp488)->f1; goto _LL579; _LL579: _temp578=(( struct Cyc_Absyn_Cast_e_struct*)
_temp488)->f2; goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp488)
== Cyc_Absyn_Address_e){ _LL583: _temp582=(( struct Cyc_Absyn_Address_e_struct*)
_temp488)->f1; goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp488)
== Cyc_Absyn_Sizeofexp_e){ _LL585: _temp584=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp488)->f1; goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp488)
== Cyc_Absyn_Deref_e){ _LL587: _temp586=(( struct Cyc_Absyn_Deref_e_struct*)
_temp488)->f1; goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp488)
== Cyc_Absyn_StructMember_e){ _LL591: _temp590=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp488)->f1; goto _LL589; _LL589: _temp588=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp488)->f2; goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp488)
== Cyc_Absyn_StructArrow_e){ _LL595: _temp594=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp488)->f1; goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp488)->f2; goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp488)
== Cyc_Absyn_Increment_e){ _LL599: _temp598=(( struct Cyc_Absyn_Increment_e_struct*)
_temp488)->f1; goto _LL597; _LL597: _temp596=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp488)->f2; goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp488)
== Cyc_Absyn_Sizeoftyp_e){ _LL601: _temp600=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp488)->f1; goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp488)
== Cyc_Absyn_Offsetof_e){ _LL605: _temp604=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp488)->f1; goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp488)->f2; goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp488)
== Cyc_Absyn_Enum_e){ _LL611: _temp610=(( struct Cyc_Absyn_Enum_e_struct*)
_temp488)->f1; goto _LL609; _LL609: _temp608=(( struct Cyc_Absyn_Enum_e_struct*)
_temp488)->f2; goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_Enum_e_struct*)
_temp488)->f3; goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp488)
== Cyc_Absyn_UnresolvedMem_e){ _LL615: _temp614=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp488)->f1; goto _LL613; _LL613: _temp612=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp488)->f2; goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp488)
== Cyc_Absyn_StmtExp_e){ _LL617: _temp616=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp488)->f1; goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL491:
return; _LL493: return; _LL495: Cyc_Tovc_exp_to_vc( env, _temp546); _temp548=
_temp544; goto _LL497; _LL497: for( 0; _temp548 != 0; _temp548=(( struct Cyc_List_List*)
_check_null( _temp548))->tl){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp548))->hd);} return; _LL499: Cyc_Tovc_exp_to_vc(
env, _temp552); _temp560= _temp556; _temp558= _temp554; goto _LL501; _LL501:
_temp564= _temp560; _temp562= _temp558; goto _LL503; _LL503: _temp570= _temp564;
_temp566= _temp562; goto _LL505; _LL505: Cyc_Tovc_exp_to_vc( env, _temp566);
_temp572= _temp570; goto _LL507; _LL507: _temp576= _temp572; goto _LL509; _LL509:
_temp578= _temp576; goto _LL511; _LL511: _temp582= _temp578; goto _LL513; _LL513:
_temp584= _temp582; goto _LL515; _LL515: _temp586= _temp584; goto _LL517; _LL517:
_temp590= _temp586; goto _LL519; _LL519: _temp594= _temp590; goto _LL521; _LL521:
_temp598= _temp594; goto _LL523; _LL523: Cyc_Tovc_exp_to_vc( env, _temp598);
return; _LL525: return; _LL527: return; _LL529: return; _LL531: for( 0; _temp612
!= 0; _temp612=(( struct Cyc_List_List*) _check_null( _temp612))->tl){ Cyc_Tovc_exp_to_vc(
env,(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp612))->hd)).f2);}
return; _LL533: Cyc_Tovc_stmt_to_vc( env, _temp616);( void*)( e->r=( void*)((
void*)( Cyc_Tovc_stmt_to_exp( env, _temp616))->r)); return; _LL535:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp618=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp618[ 0]=({ struct
Cyc_Core_Impossible_struct _temp619; _temp619.tag= Cyc_Core_Impossible; _temp619.f1=
_tag_arr("bad exp form after xlation to C", sizeof( unsigned char), 32u);
_temp619;}); _temp618;})); _LL489:;} static void Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s){ while( 1) { env.encloser= s;{ void* _temp620=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp652; struct Cyc_Absyn_Stmt* _temp654;
struct Cyc_Absyn_Stmt* _temp656; struct _tagged_arr* _temp658; struct Cyc_Absyn_Stmt*
_temp660; struct _tagged_arr* _temp662; struct Cyc_Absyn_Exp* _temp664; struct
Cyc_Absyn_Exp* _temp666; struct Cyc_Absyn_Stmt* _temp668; struct Cyc_Absyn_Stmt*
_temp670; struct Cyc_Absyn_Stmt* _temp672; struct Cyc_Absyn_Stmt* _temp674;
struct Cyc_Absyn_Exp* _temp676; struct Cyc_Absyn_Stmt* _temp678; struct _tuple2
_temp680; struct Cyc_Absyn_Stmt* _temp682; struct Cyc_Absyn_Exp* _temp684;
struct Cyc_Absyn_Stmt* _temp686; struct _tuple2 _temp688; struct Cyc_Absyn_Stmt*
_temp690; struct Cyc_Absyn_Exp* _temp692; struct _tuple2 _temp694; struct Cyc_Absyn_Stmt*
_temp696; struct Cyc_Absyn_Exp* _temp698; struct Cyc_Absyn_Exp* _temp700; struct
_tuple2 _temp702; struct Cyc_Absyn_Stmt* _temp704; struct Cyc_Absyn_Exp*
_temp706; struct Cyc_Absyn_Stmt* _temp708; struct Cyc_List_List* _temp710;
struct Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Stmt* _temp714; struct Cyc_Absyn_Decl*
_temp716; _LL622: if( _temp620 ==( void*) Cyc_Absyn_Skip_s){ goto _LL623;} else{
goto _LL624;} _LL624: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) ==
Cyc_Absyn_Break_s: 0){ _LL653: _temp652=(( struct Cyc_Absyn_Break_s_struct*)
_temp620)->f1; goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Continue_s: 0){ _LL655: _temp654=((
struct Cyc_Absyn_Continue_s_struct*) _temp620)->f1; goto _LL627;} else{ goto
_LL628;} _LL628: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Goto_s:
0){ _LL659: _temp658=(( struct Cyc_Absyn_Goto_s_struct*) _temp620)->f1; goto
_LL657; _LL657: _temp656=(( struct Cyc_Absyn_Goto_s_struct*) _temp620)->f2; goto
_LL629;} else{ goto _LL630;} _LL630: if(( unsigned int) _temp620 > 1u?*(( int*)
_temp620) == Cyc_Absyn_Label_s: 0){ _LL663: _temp662=(( struct Cyc_Absyn_Label_s_struct*)
_temp620)->f1; goto _LL661; _LL661: _temp660=(( struct Cyc_Absyn_Label_s_struct*)
_temp620)->f2; goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Return_s: 0){ _LL665: _temp664=((
struct Cyc_Absyn_Return_s_struct*) _temp620)->f1; goto _LL633;} else{ goto
_LL634;} _LL634: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Exp_s:
0){ _LL667: _temp666=(( struct Cyc_Absyn_Exp_s_struct*) _temp620)->f1; goto
_LL635;} else{ goto _LL636;} _LL636: if(( unsigned int) _temp620 > 1u?*(( int*)
_temp620) == Cyc_Absyn_Seq_s: 0){ _LL671: _temp670=(( struct Cyc_Absyn_Seq_s_struct*)
_temp620)->f1; goto _LL669; _LL669: _temp668=(( struct Cyc_Absyn_Seq_s_struct*)
_temp620)->f2; goto _LL637;} else{ goto _LL638;} _LL638: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_IfThenElse_s: 0){ _LL677:
_temp676=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp620)->f1; goto _LL675;
_LL675: _temp674=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp620)->f2; goto
_LL673; _LL673: _temp672=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp620)->f3;
goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int) _temp620 > 1u?*((
int*) _temp620) == Cyc_Absyn_While_s: 0){ _LL681: _temp680=(( struct Cyc_Absyn_While_s_struct*)
_temp620)->f1; _LL685: _temp684= _temp680.f1; goto _LL683; _LL683: _temp682=
_temp680.f2; goto _LL679; _LL679: _temp678=(( struct Cyc_Absyn_While_s_struct*)
_temp620)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_For_s: 0){ _LL701: _temp700=((
struct Cyc_Absyn_For_s_struct*) _temp620)->f1; goto _LL695; _LL695: _temp694=((
struct Cyc_Absyn_For_s_struct*) _temp620)->f2; _LL699: _temp698= _temp694.f1;
goto _LL697; _LL697: _temp696= _temp694.f2; goto _LL689; _LL689: _temp688=((
struct Cyc_Absyn_For_s_struct*) _temp620)->f3; _LL693: _temp692= _temp688.f1;
goto _LL691; _LL691: _temp690= _temp688.f2; goto _LL687; _LL687: _temp686=((
struct Cyc_Absyn_For_s_struct*) _temp620)->f4; goto _LL643;} else{ goto _LL644;}
_LL644: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Do_s:
0){ _LL709: _temp708=(( struct Cyc_Absyn_Do_s_struct*) _temp620)->f1; goto
_LL703; _LL703: _temp702=(( struct Cyc_Absyn_Do_s_struct*) _temp620)->f2; _LL707:
_temp706= _temp702.f1; goto _LL705; _LL705: _temp704= _temp702.f2; goto _LL645;}
else{ goto _LL646;} _LL646: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620)
== Cyc_Absyn_Switch_s: 0){ _LL713: _temp712=(( struct Cyc_Absyn_Switch_s_struct*)
_temp620)->f1; goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_Switch_s_struct*)
_temp620)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Decl_s: 0){ _LL717: _temp716=((
struct Cyc_Absyn_Decl_s_struct*) _temp620)->f1; goto _LL715; _LL715: _temp714=((
struct Cyc_Absyn_Decl_s_struct*) _temp620)->f2; goto _LL649;} else{ goto _LL650;}
_LL650: goto _LL651; _LL623: return; _LL625: return; _LL627: return; _LL629:
return; _LL631: s= _temp660; continue; _LL633: if( _temp664 == 0){ return;}
_temp666=( struct Cyc_Absyn_Exp*) _check_null( _temp664); goto _LL635; _LL635:
Cyc_Tovc_exp_to_vc( env, _temp666); return; _LL637: Cyc_Tovc_stmt_to_vc( env,
_temp670); s= _temp668; continue; _LL639: Cyc_Tovc_exp_to_vc( env, _temp676);
Cyc_Tovc_stmt_to_vc( env, _temp674); s= _temp672; continue; _LL641: Cyc_Tovc_exp_to_vc(
env, _temp684); s= _temp678; continue; _LL643: Cyc_Tovc_exp_to_vc( env, _temp700);
Cyc_Tovc_exp_to_vc( env, _temp698); Cyc_Tovc_exp_to_vc( env, _temp692); s=
_temp686; continue; _LL645: Cyc_Tovc_exp_to_vc( env, _temp706); s= _temp708;
continue; _LL647: Cyc_Tovc_exp_to_vc( env, _temp712); for( 0; _temp710 != 0;
_temp710=(( struct Cyc_List_List*) _check_null( _temp710))->tl){ Cyc_Tovc_stmt_to_vc(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp710))->hd)->body);} return; _LL649:{ void* _temp718=( void*) _temp716->r;
struct Cyc_Absyn_Vardecl* _temp724; _LL720: if(*(( int*) _temp718) == Cyc_Absyn_Var_d){
_LL725: _temp724=(( struct Cyc_Absyn_Var_d_struct*) _temp718)->f1; goto _LL721;}
else{ goto _LL722;} _LL722: goto _LL723; _LL721:{ void* _temp726= Cyc_Tcutil_compress((
void*) _temp724->type); struct Cyc_Absyn_Exp* _temp732; struct Cyc_Absyn_Tqual
_temp734; void* _temp736; _LL728: if(( unsigned int) _temp726 > 4u?*(( int*)
_temp726) == Cyc_Absyn_ArrayType: 0){ _LL737: _temp736=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp726)->f1; goto _LL735; _LL735: _temp734=(( struct Cyc_Absyn_ArrayType_struct*)
_temp726)->f2; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_ArrayType_struct*)
_temp726)->f3; goto _LL729;} else{ goto _LL730;} _LL730: goto _LL731; _LL729:
if( _temp724->initializer != 0){ void* _temp738=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp724->initializer))->r; struct Cyc_List_List* _temp744; struct
Cyc_Core_Opt* _temp746; _LL740: if(*(( int*) _temp738) == Cyc_Absyn_UnresolvedMem_e){
_LL747: _temp746=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp738)->f1;
goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp738)->f2; goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL741:
_temp724->initializer= 0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp(
_temp724->name, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_new_stmt(( void*)
_temp714->r, 0);{ unsigned int i= 0; for( 0; _temp744 != 0;( _temp744=(( struct
Cyc_List_List*) _check_null( _temp744))->tl, ++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp( arr, Cyc_Absyn_uint_exp( i, 0), 0),(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp744))->hd)).f2, 0),
0), s3, 0);}}( void*)( _temp714->r=( void*)(( void*) s3->r)); goto _LL739;}
_LL743:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp748=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp748[ 0]=({ struct Cyc_Core_Impossible_struct _temp749; _temp749.tag= Cyc_Core_Impossible;
_temp749.f1= _tag_arr("array type bad initializer", sizeof( unsigned char), 27u);
_temp749;}); _temp748;})); _LL739:;} goto _LL727; _LL731: goto _LL727; _LL727:;}
env.all_locals=({ struct Cyc_List_List* _temp750=( struct Cyc_List_List*)
_region_malloc( env.rgn, sizeof( struct Cyc_List_List)); _temp750->hd=( void*)
_temp724; _temp750->tl= env.all_locals; _temp750;}); if( _temp724->initializer
!= 0){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*) _check_null( _temp724->initializer));}
s= _temp714; continue; _LL723: goto _LL719; _LL719:;} goto _LL651; _LL651:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp751=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp751[ 0]=({ struct
Cyc_Core_Impossible_struct _temp752; _temp752.tag= Cyc_Core_Impossible; _temp752.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp752;});
_temp751;})); _LL621:;}}} struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List*
old_ds){ struct Cyc_List_List* _temp753= 0; goto _LL754; _LL754: for( 0; old_ds
!= 0; old_ds=(( struct Cyc_List_List*) _check_null( old_ds))->tl){ struct Cyc_Absyn_Decl*
_temp755=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( old_ds))->hd;
goto _LL756; _LL756: { void* _temp757=( void*) _temp755->r; struct Cyc_Absyn_Fndecl*
_temp783; struct Cyc_Absyn_Vardecl* _temp785; int _temp787; struct Cyc_Absyn_Exp*
_temp789; struct Cyc_Core_Opt* _temp791; struct Cyc_Core_Opt* _temp793; struct
Cyc_Absyn_Pat* _temp795; struct Cyc_List_List* _temp797; struct Cyc_Absyn_Structdecl*
_temp799; struct Cyc_Absyn_Uniondecl* _temp801; struct Cyc_Absyn_Tuniondecl*
_temp803; struct Cyc_Absyn_Enumdecl* _temp805; struct Cyc_Absyn_Typedefdecl*
_temp807; struct Cyc_List_List* _temp809; struct _tagged_arr* _temp811; struct
Cyc_List_List* _temp813; struct _tuple0* _temp815; struct Cyc_List_List*
_temp817; _LL759: if(*(( int*) _temp757) == Cyc_Absyn_Fn_d){ _LL784: _temp783=((
struct Cyc_Absyn_Fn_d_struct*) _temp757)->f1; goto _LL760;} else{ goto _LL761;}
_LL761: if(*(( int*) _temp757) == Cyc_Absyn_Var_d){ _LL786: _temp785=(( struct
Cyc_Absyn_Var_d_struct*) _temp757)->f1; goto _LL762;} else{ goto _LL763;} _LL763:
if(*(( int*) _temp757) == Cyc_Absyn_Let_d){ _LL796: _temp795=(( struct Cyc_Absyn_Let_d_struct*)
_temp757)->f1; goto _LL794; _LL794: _temp793=(( struct Cyc_Absyn_Let_d_struct*)
_temp757)->f2; goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_Let_d_struct*)
_temp757)->f3; goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Let_d_struct*)
_temp757)->f4; goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_Let_d_struct*)
_temp757)->f5; goto _LL764;} else{ goto _LL765;} _LL765: if(*(( int*) _temp757)
== Cyc_Absyn_Letv_d){ _LL798: _temp797=(( struct Cyc_Absyn_Letv_d_struct*)
_temp757)->f1; goto _LL766;} else{ goto _LL767;} _LL767: if(*(( int*) _temp757)
== Cyc_Absyn_Struct_d){ _LL800: _temp799=(( struct Cyc_Absyn_Struct_d_struct*)
_temp757)->f1; goto _LL768;} else{ goto _LL769;} _LL769: if(*(( int*) _temp757)
== Cyc_Absyn_Union_d){ _LL802: _temp801=(( struct Cyc_Absyn_Union_d_struct*)
_temp757)->f1; goto _LL770;} else{ goto _LL771;} _LL771: if(*(( int*) _temp757)
== Cyc_Absyn_Tunion_d){ _LL804: _temp803=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp757)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if(*(( int*) _temp757)
== Cyc_Absyn_Enum_d){ _LL806: _temp805=(( struct Cyc_Absyn_Enum_d_struct*)
_temp757)->f1; goto _LL774;} else{ goto _LL775;} _LL775: if(*(( int*) _temp757)
== Cyc_Absyn_Typedef_d){ _LL808: _temp807=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp757)->f1; goto _LL776;} else{ goto _LL777;} _LL777: if(*(( int*) _temp757)
== Cyc_Absyn_Namespace_d){ _LL812: _temp811=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp757)->f1; goto _LL810; _LL810: _temp809=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp757)->f2; goto _LL778;} else{ goto _LL779;} _LL779: if(*(( int*) _temp757)
== Cyc_Absyn_Using_d){ _LL816: _temp815=(( struct Cyc_Absyn_Using_d_struct*)
_temp757)->f1; goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Using_d_struct*)
_temp757)->f2; goto _LL780;} else{ goto _LL781;} _LL781: if(*(( int*) _temp757)
== Cyc_Absyn_ExternC_d){ _LL818: _temp817=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp757)->f1; goto _LL782;} else{ goto _LL758;} _LL760:{ struct _RegionHandle
_temp819= _new_region(); struct _RegionHandle* rgn=& _temp819; _push_region( rgn);{
struct Cyc_List_List** _temp821=({ struct Cyc_List_List** _temp820=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp820[ 0]= 0; _temp820;});
goto _LL822; _LL822: Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp823;
_temp823.rgn= rgn; _temp823.all_locals=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* src)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp783->param_vardecls))->v); _temp823.varmap=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle*, int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_rempty)( rgn, Cyc_String_zstrptrcmp); _temp823.encloser=
_temp783->body; _temp823.gen_ds= _temp821; _temp823;}), _temp783->body);
_temp753=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)(* _temp821, _temp753);}; _pop_region( rgn);} goto _LL762;
_LL762: goto _LL764; _LL764: goto _LL766; _LL766: goto _LL768; _LL768: goto
_LL770; _LL770: goto _LL772; _LL772: goto _LL774; _LL774: goto _LL776; _LL776:
_temp753=({ struct Cyc_List_List* _temp824=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp824->hd=( void*) _temp755; _temp824->tl=
_temp753; _temp824;}); goto _LL758; _LL778: _temp813= _temp809; goto _LL780;
_LL780: _temp817= _temp813; goto _LL782; _LL782:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp825=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp825[ 0]=({ struct
Cyc_Core_Impossible_struct _temp826; _temp826.tag= Cyc_Core_Impossible; _temp826.f1=
_tag_arr("nested translation unit after translation to C", sizeof( unsigned char),
47u); _temp826;}); _temp825;})); _LL758:;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp753);}