 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_ptrcmp(
void**, void**); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_memq( struct Cyc_List_List*
l, void* x); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u];
extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void Cyc_Dict_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr); extern int Cyc_Tcutil_bits_only(
void* t); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern
unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]; static const int Cyc_CfAbsexp_OrderedG=
0; static const int Cyc_CfAbsexp_UnorderedG= 1; static const int Cyc_CfAbsexp_OneofG=
2; extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]="\000\000\000\000BadAbsexp";
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; struct Cyc_Absyn_Vardecl* _temp10; void* _temp12;
void* _temp14; int _temp16; struct Cyc_Absyn_Exp* _temp18; _LL2: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_LocalOp: 0){ _LL11:
_temp10=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp0)->f1; goto _LL3;} else{
goto _LL4;} _LL4: if(( unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_MemberOp:
0){ _LL15: _temp14=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp0)->f1;
goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f2; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?*((
int*) _temp0) == Cyc_CfAbsexp_MallocOp: 0){ _LL19: _temp18=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp0)->f1; goto _LL17; _LL17: _temp16=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp0)->f2; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3: return; _LL5:
return; _LL7: return; _LL9:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL1:;}
static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp20= ao; struct Cyc_Absyn_Vardecl*
_temp32; void* _temp34; void* _temp36; int _temp38; struct Cyc_Absyn_Exp*
_temp40; void* _temp42; _LL22: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20)
== Cyc_CfAbsexp_LocalOp: 0){ _LL33: _temp32=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp20)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp20
> 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MemberOp: 0){ _LL37: _temp36=( void*)((
struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f1; goto _LL35; _LL35: _temp34=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f2; goto _LL25;} else{
goto _LL26;} _LL26: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MallocOp:
0){ _LL41: _temp40=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp20)->f1; goto
_LL39; _LL39: _temp38=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp20)->f2;
goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp20 > 1u?*(( int*)
_temp20) == Cyc_CfAbsexp_DerefOp: 0){ _LL43: _temp42=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp20)->f1; goto _LL29;} else{ goto _LL30;} _LL30: goto _LL31; _LL23: return;
_LL25: return; _LL27: return; _LL29: return; _LL31:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL21:;} static void Cyc_CfAbsexp_ok_deref_arg( void* ao){ void* _temp44= ao;
struct Cyc_Absyn_Vardecl* _temp54; void* _temp56; void* _temp58; void* _temp60;
_LL46: if(( unsigned int) _temp44 > 1u?*(( int*) _temp44) == Cyc_CfAbsexp_LocalOp:
0){ _LL55: _temp54=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp44)->f1; goto
_LL47;} else{ goto _LL48;} _LL48: if(( unsigned int) _temp44 > 1u?*(( int*)
_temp44) == Cyc_CfAbsexp_MemberOp: 0){ _LL59: _temp58=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp44)->f1; goto _LL57; _LL57: _temp56=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp44)->f2; goto _LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp44
> 1u?*(( int*) _temp44) == Cyc_CfAbsexp_DerefOp: 0){ _LL61: _temp60=( void*)((
struct Cyc_CfAbsexp_DerefOp_struct*) _temp44)->f1; goto _LL51;} else{ goto _LL52;}
_LL52: goto _LL53; _LL47: return; _LL49: return; _LL51: return; _LL53:( int)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL45:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp62= ao; void* _temp70; _LL64: if( _temp62 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp62 > 1u?*(( int*)
_temp62) == Cyc_CfAbsexp_AddressOp: 0){ _LL71: _temp70=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp62)->f1; goto _LL67;} else{ goto _LL68;} _LL68: goto _LL69; _LL65: goto
_LL67; _LL67:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL69: return;
_LL63:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct Cyc_Dict_Dict**
Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int( struct Cyc_Absyn_Exp*
e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*))) Cyc_Dict_empty)((
int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp); Cyc_CfAbsexp_mallocpt_dict=({
struct Cyc_Dict_Dict** _temp72=( struct Cyc_Dict_Dict**) GC_malloc( sizeof(
struct Cyc_Dict_Dict*) * 1); _temp72[ 0]= d; _temp72;});}{ int i= 0; if( !(( int(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key, int* ans_place)) Cyc_Dict_lookup_bool)(*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp73=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp73[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp74; _temp74.tag= Cyc_CfAbsexp_AddressOp;
_temp74.f1=( void*) ao; _temp74;}); _temp73;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp75=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp75[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp76; _temp76.tag= Cyc_CfAbsexp_LocalOp;
_temp76.f1= vd; _temp76;}); _temp75;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp77=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp77[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp78; _temp78.tag= Cyc_CfAbsexp_MemberOp;
_temp78.f1=( void*) ao; _temp78.f2=( void*) f; _temp78;}); _temp77;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp79=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp79[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp80; _temp80.tag= Cyc_CfAbsexp_MallocOp;
_temp80.f1= e; _temp80.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp80;}); _temp79;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp81=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp81[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp82; _temp82.tag= Cyc_CfAbsexp_DerefOp; _temp82.f1=(
void*) ao; _temp82;}); _temp81;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp83=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp83[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp84; _temp84.tag= Cyc_CfAbsexp_MallocAE;
_temp84.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp85=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp85[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp86; _temp86.tag= Cyc_CfFlowInfo_MallocPt;
_temp86.f1= e; _temp86;}); _temp85;})); _temp84.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp84;}); _temp83;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp87=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp87[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp88; _temp88.tag= Cyc_CfAbsexp_StmtAE; _temp88.f1=
s; _temp88;}); _temp87;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp89=
ao; _LL91: if( _temp89 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL92;} else{
goto _LL93;} _LL93: goto _LL94; _LL92: return( void*) Cyc_CfAbsexp_SkipAE; _LL94:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp95=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp95[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp96; _temp96.tag= Cyc_CfAbsexp_UseAE; _temp96.f1=( void*) ao; _temp96;});
_temp95;}); _LL90:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp98=({ struct _tuple3 _temp97; _temp97.f1=
l; _temp97.f2= r; _temp97;}); void* _temp106; void* _temp108; void* _temp110;
void* _temp112; _LL100: _LL109: _temp108= _temp98.f1; if( _temp108 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL107;} else{ goto _LL102;} _LL107: _temp106=
_temp98.f2; if( _temp106 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL101;} else{
goto _LL102;} _LL102: _LL113: _temp112= _temp98.f1; if( _temp112 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL111;} else{ goto _LL104;} _LL111: _temp110= _temp98.f2; goto _LL103;
_LL104: goto _LL105; _LL101: return( void*) Cyc_CfAbsexp_SkipAE; _LL103: return
Cyc_CfAbsexp_mkUseAE( r); _LL105: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp114=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp114[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp115; _temp115.tag= Cyc_CfAbsexp_AssignAE;
_temp115.f1=( void*) l; _temp115.f2=( void*) r; _temp115;}); _temp114;}); _LL99:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp117=({ struct _tuple4
_temp116; _temp116.f1= g; _temp116.f2= ae1; _temp116.f3= ae2; _temp116;}); void*
_temp140; void* _temp142; void* _temp144; void* _temp147; void* _temp149; void*
_temp151; void* _temp153; void* _temp155; void* _temp157; void* _temp159; void*
_temp161; void* _temp163; void* _temp165; void* _temp167; void* _temp169; void*
_temp172; struct Cyc_List_List* _temp174; void* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; void* _temp182; void* _temp184; void* _temp187; void*
_temp189; struct Cyc_List_List* _temp191; void* _temp193; void* _temp195; void*
_temp197; void* _temp199; struct Cyc_List_List* _temp201; void* _temp203; void*
_temp205; void* _temp208; struct Cyc_List_List* _temp210; void* _temp212; void*
_temp214; void* _temp216; _LL119: _LL145: _temp144= _temp117.f1; goto _LL143;
_LL143: _temp142= _temp117.f2; goto _LL141; _LL141: _temp140= _temp117.f3; if(
_temp140 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL139;} else{ goto _LL121;}
_LL139: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL120;} else{ goto _LL121;}
_LL121: _LL152: _temp151= _temp117.f1; goto _LL150; _LL150: _temp149= _temp117.f2;
if( _temp149 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL148;} else{ goto _LL123;}
_LL148: _temp147= _temp117.f3; goto _LL146; _LL146: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL122;} else{ goto _LL123;} _LL123: _LL158: _temp157= _temp117.f1; if(
_temp157 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL156;} else{ goto _LL125;}
_LL156: _temp155= _temp117.f2; goto _LL154; _LL154: _temp153= _temp117.f3; if(
_temp153 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL124;} else{ goto _LL125;}
_LL125: _LL164: _temp163= _temp117.f1; if( _temp163 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL162;} else{ goto _LL127;} _LL162: _temp161= _temp117.f2; if( _temp161 ==(
void*) Cyc_CfAbsexp_BottomAE){ goto _LL160;} else{ goto _LL127;} _LL160:
_temp159= _temp117.f3; goto _LL126; _LL127: _LL170: _temp169= _temp117.f1; if(
_temp169 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL168;} else{ goto _LL129;}
_LL168: _temp167= _temp117.f2; if( _temp167 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL166;} else{ goto _LL129;} _LL166: _temp165= _temp117.f3; goto _LL128;
_LL129: _LL185: _temp184= _temp117.f1; goto _LL179; _LL179: _temp178= _temp117.f2;
if(( unsigned int) _temp178 > 2u?*(( int*) _temp178) == Cyc_CfAbsexp_GroupAE: 0){
_LL183: _temp182=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp178)->f1;
goto _LL181; _LL181: _temp180=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp178)->f2;
goto _LL173;} else{ goto _LL131;} _LL173: _temp172= _temp117.f3; if((
unsigned int) _temp172 > 2u?*(( int*) _temp172) == Cyc_CfAbsexp_GroupAE: 0){
_LL177: _temp176=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp172)->f1;
goto _LL175; _LL175: _temp174=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp172)->f2;
goto _LL171;} else{ goto _LL131;} _LL171: if( _temp184 == _temp182? _temp184 ==
_temp176: 0){ goto _LL130;} else{ goto _LL131;} _LL131: _LL196: _temp195=
_temp117.f1; goto _LL190; _LL190: _temp189= _temp117.f2; if(( unsigned int)
_temp189 > 2u?*(( int*) _temp189) == Cyc_CfAbsexp_GroupAE: 0){ _LL194: _temp193=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp189)->f1; goto _LL192; _LL192:
_temp191=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp189)->f2; goto _LL188;}
else{ goto _LL133;} _LL188: _temp187= _temp117.f3; goto _LL186; _LL186: if(
_temp195 == _temp193? _temp195 !=( void*) Cyc_CfAbsexp_OrderedG: 0){ goto _LL132;}
else{ goto _LL133;} _LL133: _LL206: _temp205= _temp117.f1; if( _temp205 ==( void*)
Cyc_CfAbsexp_OrderedG){ goto _LL200;} else{ goto _LL135;} _LL200: _temp199=
_temp117.f2; if(( unsigned int) _temp199 > 2u?*(( int*) _temp199) == Cyc_CfAbsexp_GroupAE:
0){ _LL204: _temp203=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp199)->f1;
if( _temp203 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL202;} else{ goto _LL135;}
_LL202: _temp201=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp199)->f2; goto
_LL198;} else{ goto _LL135;} _LL198: _temp197= _temp117.f3; goto _LL134; _LL135:
_LL217: _temp216= _temp117.f1; goto _LL215; _LL215: _temp214= _temp117.f2; goto
_LL209; _LL209: _temp208= _temp117.f3; if(( unsigned int) _temp208 > 2u?*(( int*)
_temp208) == Cyc_CfAbsexp_GroupAE: 0){ _LL213: _temp212=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp208)->f1; goto _LL211; _LL211: _temp210=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp208)->f2; goto _LL207;} else{ goto _LL137;} _LL207: if( _temp216 ==
_temp212){ goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL120: return
ae1; _LL122: return ae2; _LL124: return ae1; _LL126: return ae2; _LL128: return(
void*) Cyc_CfAbsexp_BottomAE; _LL130: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp218=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp218[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp219; _temp219.tag= Cyc_CfAbsexp_GroupAE;
_temp219.f1=( void*) _temp184; _temp219.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp180, _temp174);
_temp219;}); _temp218;}); _LL132: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp220=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp220[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp221; _temp221.tag= Cyc_CfAbsexp_GroupAE;
_temp221.f1=( void*) _temp195; _temp221.f2=({ struct Cyc_List_List* _temp222=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp222[
0]=({ struct Cyc_List_List _temp223; _temp223.hd=( void*) ae2; _temp223.tl=
_temp191; _temp223;}); _temp222;}); _temp221;}); _temp220;}); _LL134: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp224=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp224[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp225; _temp225.tag= Cyc_CfAbsexp_GroupAE;
_temp225.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp225.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp201,({
struct Cyc_List_List* _temp226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp226[ 0]=({ struct Cyc_List_List _temp227;
_temp227.hd=( void*) ae2; _temp227.tl= 0; _temp227;}); _temp226;})); _temp225;});
_temp224;}); _LL136: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp228=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp228[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp229; _temp229.tag= Cyc_CfAbsexp_GroupAE;
_temp229.f1=( void*) _temp216; _temp229.f2=({ struct Cyc_List_List* _temp230=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp230[
0]=({ struct Cyc_List_List _temp231; _temp231.hd=( void*) ae1; _temp231.tl=
_temp210; _temp231;}); _temp230;}); _temp229;}); _temp228;}); _LL138: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp232=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp232[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp233; _temp233.tag= Cyc_CfAbsexp_GroupAE;
_temp233.f1=( void*) g; _temp233.f2=({ struct Cyc_List_List* _temp234=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp234[ 0]=({
struct Cyc_List_List _temp235; _temp235.hd=( void*) ae1; _temp235.tl=({ struct
Cyc_List_List* _temp236=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp236[ 0]=({ struct Cyc_List_List _temp237; _temp237.hd=( void*) ae2;
_temp237.tl= 0; _temp237;}); _temp236;}); _temp235;}); _temp234;}); _temp233;});
_temp232;}); _LL118:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if((( struct Cyc_List_List*)
_check_null( ael))->tl == 0){ return( void*)(( struct Cyc_List_List*)
_check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp238=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp238[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp239; _temp239.tag= Cyc_CfAbsexp_GroupAE;
_temp239.f1=( void*) g; _temp239.f2= ael; _temp239;}); _temp238;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_arr Cyc_CfAbsexp_absop2string(
void* ao){ void* _temp240= ao; void* _temp254; struct Cyc_Absyn_Vardecl*
_temp256; void* _temp258; void* _temp260; int _temp262; struct Cyc_Absyn_Exp*
_temp264; void* _temp266; _LL242: if( _temp240 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp240 > 1u?*((
int*) _temp240) == Cyc_CfAbsexp_AddressOp: 0){ _LL255: _temp254=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp240)->f1; goto _LL245;} else{ goto
_LL246;} _LL246: if(( unsigned int) _temp240 > 1u?*(( int*) _temp240) == Cyc_CfAbsexp_LocalOp:
0){ _LL257: _temp256=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp240)->f1; goto
_LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp240 > 1u?*(( int*)
_temp240) == Cyc_CfAbsexp_MemberOp: 0){ _LL261: _temp260=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp240)->f1; goto _LL259; _LL259: _temp258=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp240)->f2; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp240 > 1u?*(( int*) _temp240) == Cyc_CfAbsexp_MallocOp: 0){ _LL265: _temp264=((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp240)->f1; goto _LL263; _LL263:
_temp262=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp240)->f2; goto _LL251;}
else{ goto _LL252;} _LL252: if(( unsigned int) _temp240 > 1u?*(( int*) _temp240)
== Cyc_CfAbsexp_DerefOp: 0){ _LL267: _temp266=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp240)->f1; goto _LL253;} else{ goto _LL241;} _LL243: return _tag_arr("Unknown",
sizeof( unsigned char), 8u); _LL245: return( struct _tagged_arr)({ struct
_tagged_arr _temp268= Cyc_CfAbsexp_absop2string( _temp254); xprintf("& %.*s",
_get_arr_size( _temp268, 1u), _temp268.curr);}); _LL247: return*(* _temp256->name).f2;
_LL249: { struct _tagged_arr fs;{ void* _temp269= _temp258; struct _tagged_arr*
_temp275; int _temp277; _LL271: if(*(( int*) _temp269) == Cyc_CfFlowInfo_StructF){
_LL276: _temp275=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp269)->f1; goto
_LL272;} else{ goto _LL273;} _LL273: if(*(( int*) _temp269) == Cyc_CfFlowInfo_TupleF){
_LL278: _temp277=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp269)->f1; goto
_LL274;} else{ goto _LL270;} _LL272: fs=* _temp275; goto _LL270; _LL274: fs=(
struct _tagged_arr) xprintf("%d", _temp277); goto _LL270; _LL270:;} return(
struct _tagged_arr)({ struct _tagged_arr _temp279= Cyc_CfAbsexp_absop2string(
_temp260); struct _tagged_arr _temp280= fs; xprintf("%.*s.%.*s", _get_arr_size(
_temp279, 1u), _temp279.curr, _get_arr_size( _temp280, 1u), _temp280.curr);});}
_LL251: return( struct _tagged_arr) xprintf("<mpt%d>", _temp262); _LL253: return(
struct _tagged_arr)({ struct _tagged_arr _temp281= Cyc_CfAbsexp_absop2string(
_temp266); xprintf("(*%.*s)", _get_arr_size( _temp281, 1u), _temp281.curr);});
_LL241:;} struct _tagged_arr Cyc_CfAbsexp_absexp2string( void* ae, int depth){
struct _tagged_arr ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i <
_get_arr_size( ans, sizeof( unsigned char)); ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{ void*
_temp282= ae; void* _temp300; int _temp302; void* _temp304; struct Cyc_Absyn_Exp*
_temp306; void* _temp308; void* _temp310; struct Cyc_List_List* _temp312; void*
_temp314; struct Cyc_Absyn_Stmt* _temp316; _LL284: if( _temp282 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int) _temp282 > 2u?*((
int*) _temp282) == Cyc_CfAbsexp_UseAE: 0){ _LL301: _temp300=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp282)->f1; goto _LL287;} else{ goto _LL288;}
_LL288: if(( unsigned int) _temp282 > 2u?*(( int*) _temp282) == Cyc_CfAbsexp_MallocAE:
0){ _LL305: _temp304=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp282)->f1;
if(*(( int*) _temp304) == Cyc_CfFlowInfo_MallocPt){ _LL307: _temp306=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp304)->f1; goto _LL303;} else{ goto _LL290;}
_LL303: _temp302=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp282)->f2; goto
_LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp282 > 2u?*(( int*)
_temp282) == Cyc_CfAbsexp_AssignAE: 0){ _LL311: _temp310=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp282)->f1; goto _LL309; _LL309: _temp308=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp282)->f2; goto _LL291;} else{ goto _LL292;} _LL292: if(( unsigned int)
_temp282 > 2u?*(( int*) _temp282) == Cyc_CfAbsexp_GroupAE: 0){ _LL315: _temp314=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp282)->f1; goto _LL313; _LL313:
_temp312=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp282)->f2; goto _LL293;}
else{ goto _LL294;} _LL294: if(( unsigned int) _temp282 > 2u?*(( int*) _temp282)
== Cyc_CfAbsexp_StmtAE: 0){ _LL317: _temp316=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp282)->f1; goto _LL295;} else{ goto _LL296;} _LL296: if( _temp282 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL297;} else{ goto _LL298;} _LL298: goto _LL299;
_LL285: ans=({ struct _tagged_arr _temp318=( struct _tagged_arr) ans; xprintf("%.*sskip",
_get_arr_size( _temp318, 1u), _temp318.curr);}); goto _LL283; _LL287: ans=({
struct _tagged_arr _temp319=( struct _tagged_arr) ans; struct _tagged_arr
_temp320= Cyc_CfAbsexp_absop2string( _temp300); xprintf("%.*suse %.*s",
_get_arr_size( _temp319, 1u), _temp319.curr, _get_arr_size( _temp320, 1u),
_temp320.curr);}); goto _LL283; _LL289: ans=({ struct _tagged_arr _temp321=(
struct _tagged_arr) ans; int _temp322= _temp302; xprintf("%.*smalloc %i",
_get_arr_size( _temp321, 1u), _temp321.curr, _temp322);}); goto _LL283; _LL291:
ans=({ struct _tagged_arr _temp323=( struct _tagged_arr) ans; struct _tagged_arr
_temp324= Cyc_CfAbsexp_absop2string( _temp310); struct _tagged_arr _temp325= Cyc_CfAbsexp_absop2string(
_temp308); xprintf("%.*sassign %.*s %.*s", _get_arr_size( _temp323, 1u),
_temp323.curr, _get_arr_size( _temp324, 1u), _temp324.curr, _get_arr_size(
_temp325, 1u), _temp325.curr);}); goto _LL283; _LL293:{ void* _temp326= _temp314;
_LL328: if( _temp326 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL329;} else{ goto
_LL330;} _LL330: if( _temp326 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL331;}
else{ goto _LL332;} _LL332: if( _temp326 ==( void*) Cyc_CfAbsexp_OneofG){ goto
_LL333;} else{ goto _LL327;} _LL329: ans=({ struct _tagged_arr _temp334=( struct
_tagged_arr) ans; xprintf("%.*sordered", _get_arr_size( _temp334, 1u), _temp334.curr);});
goto _LL327; _LL331: ans=({ struct _tagged_arr _temp335=( struct _tagged_arr)
ans; xprintf("%.*sunordered", _get_arr_size( _temp335, 1u), _temp335.curr);});
goto _LL327; _LL333: ans=({ struct _tagged_arr _temp336=( struct _tagged_arr)
ans; xprintf("%.*soneof", _get_arr_size( _temp336, 1u), _temp336.curr);}); goto
_LL327; _LL327:;} for( 0; _temp312 != 0; _temp312=(( struct Cyc_List_List*)
_check_null( _temp312))->tl){ ans=({ struct _tagged_arr _temp337=( struct
_tagged_arr) ans; struct _tagged_arr _temp338= Cyc_CfAbsexp_absexp2string(( void*)((
struct Cyc_List_List*) _check_null( _temp312))->hd, depth + 1); xprintf("%.*s\n%.*s",
_get_arr_size( _temp337, 1u), _temp337.curr, _get_arr_size( _temp338, 1u),
_temp338.curr);});} goto _LL283; _LL295: ans=({ struct _tagged_arr _temp339=(
struct _tagged_arr) ans; xprintf("%.*s<<stmt>>", _get_arr_size( _temp339, 1u),
_temp339.curr);}); goto _LL283; _LL297: ans=({ struct _tagged_arr _temp340=(
struct _tagged_arr) ans; xprintf("%.*sbottom", _get_arr_size( _temp340, 1u),
_temp340.curr);}); goto _LL283; _LL299:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL283:;} return( struct _tagged_arr) ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp341= ao; void* _temp355; void* _temp357; void* _temp359;
void* _temp361; struct Cyc_Absyn_Vardecl* _temp363; int _temp365; struct Cyc_Absyn_Exp*
_temp367; _LL343: if( _temp341 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL344;}
else{ goto _LL345;} _LL345: if(( unsigned int) _temp341 > 1u?*(( int*) _temp341)
== Cyc_CfAbsexp_AddressOp: 0){ _LL356: _temp355=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp341)->f1; goto _LL346;} else{ goto _LL347;} _LL347: if(( unsigned int)
_temp341 > 1u?*(( int*) _temp341) == Cyc_CfAbsexp_DerefOp: 0){ _LL358: _temp357=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp341)->f1; goto _LL348;} else{
goto _LL349;} _LL349: if(( unsigned int) _temp341 > 1u?*(( int*) _temp341) ==
Cyc_CfAbsexp_MemberOp: 0){ _LL362: _temp361=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp341)->f1; goto _LL360; _LL360: _temp359=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp341)->f2; goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int)
_temp341 > 1u?*(( int*) _temp341) == Cyc_CfAbsexp_LocalOp: 0){ _LL364: _temp363=((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp341)->f1; goto _LL352;} else{ goto
_LL353;} _LL353: if(( unsigned int) _temp341 > 1u?*(( int*) _temp341) == Cyc_CfAbsexp_MallocOp:
0){ _LL368: _temp367=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp341)->f1;
goto _LL366; _LL366: _temp365=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp341)->f2;
goto _LL354;} else{ goto _LL342;} _LL344: return; _LL346: Cyc_CfAbsexp_check_absop(
_temp355); Cyc_CfAbsexp_ok_address_arg( _temp355); return; _LL348: Cyc_CfAbsexp_check_absop(
_temp357); Cyc_CfAbsexp_ok_deref_arg( _temp357); return; _LL350: Cyc_CfAbsexp_check_absop(
_temp361); Cyc_CfAbsexp_ok_member_arg( _temp361); return; _LL352: return; _LL354:
return; _LL342:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp369= ae;
int _temp387; void* _temp389; struct Cyc_Absyn_Vardecl* _temp391; int _temp393;
void* _temp395; struct Cyc_Absyn_Stmt* _temp397; struct Cyc_List_List* _temp399;
void* _temp401; void* _temp403; void* _temp405; void* _temp407; _LL371: if(
_temp369 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL372;} else{ goto _LL373;}
_LL373: if( _temp369 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL374;} else{ goto
_LL375;} _LL375: if(( unsigned int) _temp369 > 2u?*(( int*) _temp369) == Cyc_CfAbsexp_MallocAE:
0){ _LL390: _temp389=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp369)->f1;
if(*(( int*) _temp389) == Cyc_CfFlowInfo_VarRoot){ _LL392: _temp391=(( struct
Cyc_CfFlowInfo_VarRoot_struct*) _temp389)->f1; goto _LL388;} else{ goto _LL377;}
_LL388: _temp387=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp369)->f2; goto
_LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp369 > 2u?*(( int*)
_temp369) == Cyc_CfAbsexp_MallocAE: 0){ _LL396: _temp395=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp369)->f1; goto _LL394; _LL394: _temp393=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp369)->f2; goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int)
_temp369 > 2u?*(( int*) _temp369) == Cyc_CfAbsexp_StmtAE: 0){ _LL398: _temp397=((
struct Cyc_CfAbsexp_StmtAE_struct*) _temp369)->f1; goto _LL380;} else{ goto
_LL381;} _LL381: if(( unsigned int) _temp369 > 2u?*(( int*) _temp369) == Cyc_CfAbsexp_GroupAE:
0){ _LL402: _temp401=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp369)->f1;
goto _LL400; _LL400: _temp399=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp369)->f2;
goto _LL382;} else{ goto _LL383;} _LL383: if(( unsigned int) _temp369 > 2u?*((
int*) _temp369) == Cyc_CfAbsexp_UseAE: 0){ _LL404: _temp403=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp369)->f1; goto _LL384;} else{ goto _LL385;}
_LL385: if(( unsigned int) _temp369 > 2u?*(( int*) _temp369) == Cyc_CfAbsexp_AssignAE:
0){ _LL408: _temp407=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp369)->f1;
goto _LL406; _LL406: _temp405=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp369)->f2; goto _LL386;} else{ goto _LL370;} _LL372: return; _LL374: return;
_LL376:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL378: return; _LL380:
return; _LL382: for( 0; _temp399 != 0; _temp399=(( struct Cyc_List_List*)
_check_null( _temp399))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp399))->hd);} return; _LL384: Cyc_CfAbsexp_check_absop(
_temp403); return; _LL386: Cyc_CfAbsexp_check_absop( _temp407); Cyc_CfAbsexp_check_absop(
_temp405); Cyc_CfAbsexp_ok_lvalue( _temp407); return; _LL370:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp409= ao; struct Cyc_Absyn_Vardecl*
_temp423; int _temp425; struct Cyc_Absyn_Exp* _temp427; void* _temp429; void*
_temp431; void* _temp433; void* _temp435; _LL411: if(( unsigned int) _temp409 >
1u?*(( int*) _temp409) == Cyc_CfAbsexp_LocalOp: 0){ _LL424: _temp423=(( struct
Cyc_CfAbsexp_LocalOp_struct*) _temp409)->f1; goto _LL412;} else{ goto _LL413;}
_LL413: if(( unsigned int) _temp409 > 1u?*(( int*) _temp409) == Cyc_CfAbsexp_MallocOp:
0){ _LL428: _temp427=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp409)->f1;
goto _LL426; _LL426: _temp425=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp409)->f2;
goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp409 > 1u?*((
int*) _temp409) == Cyc_CfAbsexp_MemberOp: 0){ _LL432: _temp431=( void*)(( struct
Cyc_CfAbsexp_MemberOp_struct*) _temp409)->f1; goto _LL430; _LL430: _temp429=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp409)->f2; goto _LL416;}
else{ goto _LL417;} _LL417: if(( unsigned int) _temp409 > 1u?*(( int*) _temp409)
== Cyc_CfAbsexp_DerefOp: 0){ _LL434: _temp433=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp409)->f1; goto _LL418;} else{ goto _LL419;} _LL419: if( _temp409 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL420;} else{ goto _LL421;} _LL421: if((
unsigned int) _temp409 > 1u?*(( int*) _temp409) == Cyc_CfAbsexp_AddressOp: 0){
_LL436: _temp435=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp409)->f1;
goto _LL422;} else{ goto _LL410;} _LL412: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp437=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp437[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp438; _temp438.tag= Cyc_CfFlowInfo_MustPointTo; _temp438.f1=({ struct Cyc_CfFlowInfo_Place*
_temp439=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp439[ 0]=({ struct Cyc_CfFlowInfo_Place _temp440; _temp440.root=( void*)((
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp441=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp441[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp442; _temp442.tag= Cyc_CfFlowInfo_VarRoot;
_temp442.f1= _temp423; _temp442;}); _temp441;})); _temp440.fields= 0; _temp440;});
_temp439;}); _temp438;}); _temp437;}); _LL414: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp443=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp443[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp444; _temp444.tag= Cyc_CfFlowInfo_MustPointTo; _temp444.f1=({ struct Cyc_CfFlowInfo_Place*
_temp445=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp445[ 0]=({ struct Cyc_CfFlowInfo_Place _temp446; _temp446.root=( void*)((
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp447=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp447[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp448; _temp448.tag= Cyc_CfFlowInfo_MallocPt;
_temp448.f1= _temp427; _temp448;}); _temp447;})); _temp446.fields= 0; _temp446;});
_temp445;}); _temp444;}); _temp443;}); _LL416: { void* _temp449= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp431); goto _LL450; _LL450: { void* _temp451= _temp449; void*
_temp457; void* _temp459; struct Cyc_CfFlowInfo_Place* _temp461; struct Cyc_CfFlowInfo_Place
_temp463; struct Cyc_List_List* _temp464; void* _temp466; _LL453: if(*(( int*)
_temp451) == Cyc_CfFlowInfo_UnknownIS){ _LL460: _temp459=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp451)->f1; goto _LL458; _LL458: _temp457=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp451)->f2; goto _LL454;} else{ goto _LL455;} _LL455: if(*(( int*) _temp451)
== Cyc_CfFlowInfo_MustPointTo){ _LL462: _temp461=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp451)->f1; _temp463=* _temp461; _LL467: _temp466=( void*) _temp463.root;
goto _LL465; _LL465: _temp464= _temp463.fields; goto _LL456;} else{ goto _LL452;}
_LL454: return _temp449; _LL456: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp468=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp468[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp469; _temp469.tag= Cyc_CfFlowInfo_MustPointTo; _temp469.f1=({ struct Cyc_CfFlowInfo_Place*
_temp470=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp470[ 0]=({ struct Cyc_CfFlowInfo_Place _temp471; _temp471.root=( void*)
_temp466; _temp471.fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp464,({ struct Cyc_List_List*
_temp472=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp472[ 0]=({ struct Cyc_List_List _temp473; _temp473.hd=( void*) _temp429;
_temp473.tl= 0; _temp473;}); _temp472;})); _temp471;}); _temp470;}); _temp469;});
_temp468;}); _LL452:;}} _LL418: { void* _temp474= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp433); goto _LL475; _LL475: { void* _temp476= _temp474; void*
_temp484; struct Cyc_Dict_Dict* _temp486; struct Cyc_Dict_Dict* _temp488; _LL478:
if(*(( int*) _temp476) == Cyc_CfFlowInfo_LeafPI){ _LL485: _temp484=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp476)->f1; goto _LL479;} else{ goto
_LL480;} _LL480: if(*(( int*) _temp476) == Cyc_CfFlowInfo_TuplePI){ _LL487:
_temp486=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp476)->f1; goto _LL481;}
else{ goto _LL482;} _LL482: if(*(( int*) _temp476) == Cyc_CfFlowInfo_StructPI){
_LL489: _temp488=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp476)->f1; goto
_LL483;} else{ goto _LL477;} _LL479: return _temp484; _LL481: goto _LL483;
_LL483:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL477:;}} _LL420: goto
_LL422; _LL422:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL410:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp490= ao; struct Cyc_Absyn_Vardecl* _temp504; int _temp506; struct Cyc_Absyn_Exp*
_temp508; void* _temp510; void* _temp512; void* _temp514; void* _temp516; _LL492:
if(( unsigned int) _temp490 > 1u?*(( int*) _temp490) == Cyc_CfAbsexp_LocalOp: 0){
_LL505: _temp504=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp490)->f1; goto
_LL493;} else{ goto _LL494;} _LL494: if(( unsigned int) _temp490 > 1u?*(( int*)
_temp490) == Cyc_CfAbsexp_MallocOp: 0){ _LL509: _temp508=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp490)->f1; goto _LL507; _LL507: _temp506=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp490)->f2; goto _LL495;} else{ goto _LL496;} _LL496: if(( unsigned int)
_temp490 > 1u?*(( int*) _temp490) == Cyc_CfAbsexp_MemberOp: 0){ _LL513: _temp512=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp490)->f1; goto _LL511;
_LL511: _temp510=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp490)->f2;
goto _LL497;} else{ goto _LL498;} _LL498: if(( unsigned int) _temp490 > 1u?*((
int*) _temp490) == Cyc_CfAbsexp_DerefOp: 0){ _LL515: _temp514=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp490)->f1; goto _LL499;} else{ goto _LL500;}
_LL500: if( _temp490 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL501;} else{
goto _LL502;} _LL502: if(( unsigned int) _temp490 > 1u?*(( int*) _temp490) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL517: _temp516=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp490)->f1; goto _LL503;} else{ goto _LL491;} _LL493: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp518=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp518[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp519; _temp519.tag= Cyc_CfFlowInfo_VarRoot;
_temp519.f1= _temp504; _temp519;}); _temp518;})); _LL495: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp520=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp520[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp521; _temp521.tag= Cyc_CfFlowInfo_MallocPt;
_temp521.f1= _temp508; _temp521;}); _temp520;})); _LL497: { void* _temp522= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp512); goto _LL523; _LL523: { struct _tuple3 _temp525=({ struct
_tuple3 _temp524; _temp524.f1= _temp522; _temp524.f2= _temp510; _temp524;});
void* _temp535; void* _temp537; void* _temp539; void* _temp541; void* _temp543;
void* _temp545; int _temp547; void* _temp549; struct Cyc_Dict_Dict* _temp551;
void* _temp553; struct _tagged_arr* _temp555; void* _temp557; struct Cyc_Dict_Dict*
_temp559; _LL527: _LL538: _temp537= _temp525.f1; if(*(( int*) _temp537) == Cyc_CfFlowInfo_LeafPI){
_LL540: _temp539=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp537)->f1;
if(*(( int*) _temp539) == Cyc_CfFlowInfo_UnknownIS){ _LL544: _temp543=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp539)->f1; goto _LL542; _LL542:
_temp541=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp539)->f2; goto
_LL536;} else{ goto _LL529;}} else{ goto _LL529;} _LL536: _temp535= _temp525.f2;
goto _LL528; _LL529: _LL550: _temp549= _temp525.f1; if(*(( int*) _temp549) ==
Cyc_CfFlowInfo_TuplePI){ _LL552: _temp551=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp549)->f1; goto _LL546;} else{ goto _LL531;} _LL546: _temp545= _temp525.f2;
if(*(( int*) _temp545) == Cyc_CfFlowInfo_TupleF){ _LL548: _temp547=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp545)->f1; goto _LL530;} else{ goto _LL531;} _LL531: _LL558: _temp557=
_temp525.f1; if(*(( int*) _temp557) == Cyc_CfFlowInfo_StructPI){ _LL560:
_temp559=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp557)->f1; goto _LL554;}
else{ goto _LL533;} _LL554: _temp553= _temp525.f2; if(*(( int*) _temp553) == Cyc_CfFlowInfo_StructF){
_LL556: _temp555=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp553)->f1; goto
_LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL528: return _temp522;
_LL530: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp551, _temp547); _LL532: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp559, _temp555); _LL534:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL526:;}} _LL499: { void* _temp561= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp514); goto _LL562; _LL562: { void* _temp563= _temp561; void*
_temp573; void* _temp575; void* _temp577; void* _temp579; void* _temp581; void*
_temp583; void* _temp585; struct Cyc_CfFlowInfo_Place* _temp587; _LL565: if(*((
int*) _temp563) == Cyc_CfFlowInfo_LeafPI){ _LL574: _temp573=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp563)->f1; if(*(( int*) _temp573) == Cyc_CfFlowInfo_UnknownIS){
_LL578: _temp577=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp573)->f1;
goto _LL576; _LL576: _temp575=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp573)->f2; if( _temp575 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL566;}
else{ goto _LL567;}} else{ goto _LL567;}} else{ goto _LL567;} _LL567: if(*(( int*)
_temp563) == Cyc_CfFlowInfo_LeafPI){ _LL580: _temp579=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp563)->f1; if(*(( int*) _temp579) == Cyc_CfFlowInfo_UnknownIS){ _LL584:
_temp583=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp579)->f1; goto
_LL582; _LL582: _temp581=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp579)->f2; goto _LL568;} else{ goto _LL569;}} else{ goto _LL569;} _LL569:
if(*(( int*) _temp563) == Cyc_CfFlowInfo_LeafPI){ _LL586: _temp585=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp563)->f1; if(*(( int*) _temp585) ==
Cyc_CfFlowInfo_MustPointTo){ _LL588: _temp587=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp585)->f1; goto _LL570;} else{ goto _LL571;}} else{ goto _LL571;} _LL571:
goto _LL572; _LL566: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_NoneIL); _LL568: return _temp561; _LL570: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp587); _LL572:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL564:;}} _LL501: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL); _LL503: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp589=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp589[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp590; _temp590.tag= Cyc_CfFlowInfo_LeafPI;
_temp590.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp516); _temp590;});
_temp589;}); _LL491:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp592=({
struct _tuple3 _temp591; _temp591.f1= old_pinfo; _temp591.f2= new_pinfo;
_temp591;}); void* _temp604; void* _temp606; void* _temp608; void* _temp610;
void* _temp612; void* _temp614; void* _temp616; void* _temp618; void* _temp620;
void* _temp622; void* _temp624; struct Cyc_Dict_Dict* _temp626; void* _temp628;
struct Cyc_Dict_Dict* _temp630; void* _temp632; struct Cyc_Dict_Dict* _temp634;
void* _temp636; struct Cyc_Dict_Dict* _temp638; _LL594: _LL609: _temp608=
_temp592.f1; if(*(( int*) _temp608) == Cyc_CfFlowInfo_LeafPI){ _LL611: _temp610=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp608)->f1; if(*(( int*)
_temp610) == Cyc_CfFlowInfo_UnknownIS){ _LL615: _temp614=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp610)->f1; if( _temp614 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL613;} else{
goto _LL596;} _LL613: _temp612=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp610)->f2; goto _LL605;} else{ goto _LL596;}} else{ goto _LL596;} _LL605:
_temp604= _temp592.f2; if(*(( int*) _temp604) == Cyc_CfFlowInfo_LeafPI){ _LL607:
_temp606=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp604)->f1; goto
_LL595;} else{ goto _LL596;} _LL596: _LL621: _temp620= _temp592.f1; if(*(( int*)
_temp620) == Cyc_CfFlowInfo_LeafPI){ _LL623: _temp622=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp620)->f1; goto _LL617;} else{ goto _LL598;} _LL617: _temp616= _temp592.f2;
if(*(( int*) _temp616) == Cyc_CfFlowInfo_LeafPI){ _LL619: _temp618=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp616)->f1; goto _LL597;} else{ goto
_LL598;} _LL598: _LL629: _temp628= _temp592.f1; if(*(( int*) _temp628) == Cyc_CfFlowInfo_TuplePI){
_LL631: _temp630=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp628)->f1; goto
_LL625;} else{ goto _LL600;} _LL625: _temp624= _temp592.f2; if(*(( int*)
_temp624) == Cyc_CfFlowInfo_TuplePI){ _LL627: _temp626=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp624)->f1; goto _LL599;} else{ goto _LL600;} _LL600: _LL637: _temp636=
_temp592.f1; if(*(( int*) _temp636) == Cyc_CfFlowInfo_StructPI){ _LL639:
_temp638=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp636)->f1; goto _LL633;}
else{ goto _LL602;} _LL633: _temp632= _temp592.f2; if(*(( int*) _temp632) == Cyc_CfFlowInfo_StructPI){
_LL635: _temp634=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp632)->f1; goto
_LL601;} else{ goto _LL602;} _LL602: goto _LL603; _LL595:* escaping_states=({
struct Cyc_List_List* _temp640=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp640[ 0]=({ struct Cyc_List_List _temp641;
_temp641.hd=( void*) new_pinfo; _temp641.tl=* escaping_states; _temp641;});
_temp640;}); return; _LL597: return; _LL599:(( void(*)( void(* f)( struct Cyc_List_List**,
void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp630,
_temp626); return; _LL601:(( void(*)( void(* f)( struct Cyc_List_List**, void*,
void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp638,
_temp634); return; _LL603:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp642=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp642[ 0]=({ struct Cyc_Core_Impossible_struct _temp643; _temp643.tag= Cyc_Core_Impossible;
_temp643.f1= _tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u);
_temp643;}); _temp642;})); _LL593:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void*
lval, struct Cyc_List_List* ok_mallocs){ void* _temp644= lval; int _temp652;
struct Cyc_Absyn_Exp* _temp654; void* _temp656; void* _temp658; _LL646: if((
unsigned int) _temp644 > 1u?*(( int*) _temp644) == Cyc_CfAbsexp_MallocOp: 0){
_LL655: _temp654=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp644)->f1; goto
_LL653; _LL653: _temp652=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp644)->f2;
goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int) _temp644 > 1u?*((
int*) _temp644) == Cyc_CfAbsexp_MemberOp: 0){ _LL659: _temp658=( void*)(( struct
Cyc_CfAbsexp_MemberOp_struct*) _temp644)->f1; goto _LL657; _LL657: _temp656=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp644)->f2; goto _LL649;}
else{ goto _LL650;} _LL650: goto _LL651; _LL647: return(( int(*)( struct Cyc_List_List*
l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)( ok_mallocs, _temp654); _LL649:
return Cyc_CfAbsexp_is_ok_malloc_assign( _temp658, ok_mallocs); _LL651: return 0;
_LL645:;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs( struct _RegionHandle*
rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* _temp660=
0; goto _LL661; _LL661: { struct Cyc_List_List* _temp662= 0; goto _LL663; _LL663:{
void* _temp664= ae; void* _temp686; void* _temp688; void* _temp690; int _temp692;
void* _temp694; struct Cyc_Absyn_Exp* _temp696; int _temp698; void* _temp700;
struct Cyc_Absyn_Stmt* _temp702; struct Cyc_List_List* _temp704; void* _temp706;
struct Cyc_List_List* _temp708; void* _temp710; struct Cyc_List_List* _temp712;
void* _temp714; _LL666: if( _temp664 ==( void*) Cyc_CfAbsexp_BottomAE){ goto
_LL667;} else{ goto _LL668;} _LL668: if( _temp664 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int) _temp664 > 2u?*((
int*) _temp664) == Cyc_CfAbsexp_UseAE: 0){ _LL687: _temp686=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp664)->f1; goto _LL671;} else{ goto _LL672;}
_LL672: if(( unsigned int) _temp664 > 2u?*(( int*) _temp664) == Cyc_CfAbsexp_AssignAE:
0){ _LL691: _temp690=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp664)->f1;
goto _LL689; _LL689: _temp688=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp664)->f2; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp664 > 2u?*(( int*) _temp664) == Cyc_CfAbsexp_MallocAE: 0){ _LL695: _temp694=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp664)->f1; if(*(( int*)
_temp694) == Cyc_CfFlowInfo_MallocPt){ _LL697: _temp696=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp694)->f1; goto _LL693;} else{ goto _LL676;} _LL693: _temp692=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp664)->f2; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp664 > 2u?*(( int*) _temp664) == Cyc_CfAbsexp_MallocAE: 0){ _LL701: _temp700=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp664)->f1; goto _LL699;
_LL699: _temp698=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp664)->f2; goto
_LL677;} else{ goto _LL678;} _LL678: if(( unsigned int) _temp664 > 2u?*(( int*)
_temp664) == Cyc_CfAbsexp_StmtAE: 0){ _LL703: _temp702=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp664)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp664 > 2u?*(( int*) _temp664) == Cyc_CfAbsexp_GroupAE: 0){ _LL707: _temp706=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp664)->f1; if( _temp706 ==(
void*) Cyc_CfAbsexp_OrderedG){ goto _LL705;} else{ goto _LL682;} _LL705:
_temp704=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp664)->f2; goto _LL681;}
else{ goto _LL682;} _LL682: if(( unsigned int) _temp664 > 2u?*(( int*) _temp664)
== Cyc_CfAbsexp_GroupAE: 0){ _LL711: _temp710=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp664)->f1; if( _temp710 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL709;} else{
goto _LL684;} _LL709: _temp708=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp664)->f2;
goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int) _temp664 > 2u?*((
int*) _temp664) == Cyc_CfAbsexp_GroupAE: 0){ _LL715: _temp714=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp664)->f1; if( _temp714 ==( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL713;} else{ goto _LL665;} _LL713: _temp712=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp664)->f2; goto _LL685;} else{ goto _LL665;} _LL667: goto _LL665; _LL669:
goto _LL665; _LL671: goto _LL665; _LL673: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp690, ok_mallocs)){ _temp660=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp716->hd=( void*) ae;
_temp716->tl= _temp660; _temp716;});} goto _LL665; _LL675: _temp662=({ struct
Cyc_List_List* _temp717=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp717->hd=( void*) _temp696; _temp717->tl= _temp662;
_temp717;}); goto _LL665; _LL677:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp718=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp718[ 0]=({ struct Cyc_Core_Impossible_struct _temp719; _temp719.tag= Cyc_Core_Impossible;
_temp719.f1= _tag_arr("local variable in MallocAE", sizeof( unsigned char), 27u);
_temp719;}); _temp718;})); _LL679: goto _LL665; _LL681: for( 0; _temp704 != 0;
_temp704=(( struct Cyc_List_List*) _check_null( _temp704))->tl){ struct Cyc_List_List*
_temp722; struct Cyc_List_List* _temp724; struct _tuple5 _temp720= Cyc_CfAbsexp_unordered_mallocs(
rgn,( void*)(( struct Cyc_List_List*) _check_null( _temp704))->hd,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp662, ok_mallocs)); _LL725: _temp724= _temp720.f1; goto _LL723; _LL723:
_temp722= _temp720.f2; goto _LL721; _LL721: _temp660=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp724, _temp660); _temp662=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp722, _temp662);} goto _LL665; _LL683: goto _LL665; _LL685: for( 0;
_temp712 != 0; _temp712=(( struct Cyc_List_List*) _check_null( _temp712))->tl){
struct Cyc_List_List* _temp728; struct Cyc_List_List* _temp730; struct _tuple5
_temp726= Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)(( struct Cyc_List_List*)
_check_null( _temp712))->hd, ok_mallocs); _LL731: _temp730= _temp726.f1; goto
_LL729; _LL729: _temp728= _temp726.f2; goto _LL727; _LL727: _temp660=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rappend)( rgn, _temp730, _temp660); _temp662=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp728, _temp662);} goto _LL665; _LL665:;} return({ struct _tuple5
_temp732; _temp732.f1= _temp660; _temp732.f2= _temp662; _temp732;});}} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp733= in_flow;
struct Cyc_Dict_Dict* _temp739; _LL735: if( _temp733 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL736;} else{ goto _LL737;} _LL737: if(( unsigned int) _temp733 > 1u?*((
int*) _temp733) == Cyc_CfFlowInfo_InitsFL: 0){ _LL740: _temp739=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp733)->f1; goto _LL738;} else{ goto _LL734;} _LL736: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL738: pinfo_dict= _temp739; goto _LL734; _LL734:;}{ void* _temp741= ae; void*
_temp761; void* _temp763; void* _temp765; int _temp767; void* _temp769; struct
Cyc_Absyn_Stmt* _temp771; struct Cyc_List_List* _temp773; void* _temp775; struct
Cyc_List_List* _temp777; void* _temp779; struct Cyc_List_List* _temp781; void*
_temp783; _LL743: if(( unsigned int) _temp741 > 2u?*(( int*) _temp741) == Cyc_CfAbsexp_AssignAE:
0){ _LL764: _temp763=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp741)->f1;
goto _LL762; _LL762: _temp761=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp741)->f2; goto _LL744;} else{ goto _LL745;} _LL745: if(( unsigned int)
_temp741 > 2u?*(( int*) _temp741) == Cyc_CfAbsexp_UseAE: 0){ _LL766: _temp765=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp741)->f1; goto _LL746;} else{
goto _LL747;} _LL747: if(( unsigned int) _temp741 > 2u?*(( int*) _temp741) ==
Cyc_CfAbsexp_MallocAE: 0){ _LL770: _temp769=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp741)->f1; goto _LL768; _LL768: _temp767=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp741)->f2; goto _LL748;} else{ goto _LL749;} _LL749: if( _temp741 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL750;} else{ goto _LL751;} _LL751: if( _temp741
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL752;} else{ goto _LL753;} _LL753: if((
unsigned int) _temp741 > 2u?*(( int*) _temp741) == Cyc_CfAbsexp_StmtAE: 0){
_LL772: _temp771=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp741)->f1; goto
_LL754;} else{ goto _LL755;} _LL755: if(( unsigned int) _temp741 > 2u?*(( int*)
_temp741) == Cyc_CfAbsexp_GroupAE: 0){ _LL776: _temp775=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp741)->f1; if( _temp775 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL774;}
else{ goto _LL757;} _LL774: _temp773=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp741)->f2; goto _LL756;} else{ goto _LL757;} _LL757: if(( unsigned int)
_temp741 > 2u?*(( int*) _temp741) == Cyc_CfAbsexp_GroupAE: 0){ _LL780: _temp779=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp741)->f1; if( _temp779 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL778;} else{ goto _LL759;} _LL778: _temp777=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp741)->f2; goto _LL758;} else{ goto
_LL759;} _LL759: if(( unsigned int) _temp741 > 2u?*(( int*) _temp741) == Cyc_CfAbsexp_GroupAE:
0){ _LL784: _temp783=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp741)->f1;
if( _temp783 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL782;} else{ goto
_LL742;} _LL782: _temp781=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp741)->f2;
goto _LL760;} else{ goto _LL742;} _LL744:{ void* _temp785= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp763); void* _temp791; void* _temp793; struct Cyc_CfFlowInfo_Place*
_temp795; _LL787: if(*(( int*) _temp785) == Cyc_CfFlowInfo_UnknownIS){ _LL794:
_temp793=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp785)->f1; goto
_LL792; _LL792: _temp791=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp785)->f2; goto _LL788;} else{ goto _LL789;} _LL789: if(*(( int*) _temp785)
== Cyc_CfFlowInfo_MustPointTo){ _LL796: _temp795=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp785)->f1; goto _LL790;} else{ goto _LL786;} _LL788: goto _LL786; _LL790: {
void* _temp797= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp795); goto _LL798;
_LL798: { void* _temp799= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp761); goto
_LL800; _LL800: { struct _tuple3 _temp802=({ struct _tuple3 _temp801; _temp801.f1=
_temp797; _temp801.f2= _temp799; _temp801;}); void* _temp810; void* _temp812;
void* _temp814; void* _temp816; void* _temp818; struct Cyc_Dict_Dict* _temp820;
void* _temp822; void* _temp824; void* _temp826; void* _temp828; void* _temp830;
struct Cyc_Dict_Dict* _temp832; _LL804: _LL819: _temp818= _temp802.f1; if(*((
int*) _temp818) == Cyc_CfFlowInfo_TuplePI){ _LL821: _temp820=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp818)->f1; goto _LL811;} else{ goto _LL806;} _LL811: _temp810= _temp802.f2;
if(*(( int*) _temp810) == Cyc_CfFlowInfo_LeafPI){ _LL813: _temp812=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp810)->f1; if(*(( int*) _temp812) ==
Cyc_CfFlowInfo_UnknownIS){ _LL817: _temp816=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp812)->f1; goto _LL815; _LL815: _temp814=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp812)->f2; goto _LL805;} else{ goto _LL806;}} else{ goto _LL806;} _LL806:
_LL831: _temp830= _temp802.f1; if(*(( int*) _temp830) == Cyc_CfFlowInfo_StructPI){
_LL833: _temp832=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp830)->f1; goto
_LL823;} else{ goto _LL808;} _LL823: _temp822= _temp802.f2; if(*(( int*)
_temp822) == Cyc_CfFlowInfo_LeafPI){ _LL825: _temp824=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp822)->f1; if(*(( int*) _temp824) == Cyc_CfFlowInfo_UnknownIS){ _LL829:
_temp828=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp824)->f1; goto
_LL827; _LL827: _temp826=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp824)->f2; goto _LL807;} else{ goto _LL808;}} else{ goto _LL808;} _LL808:
goto _LL809; _LL805: _temp826= _temp814; goto _LL807; _LL807: _temp799= Cyc_CfFlowInfo_assign_unknown_dict(
_temp826, _temp797); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp795, _temp799);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp836=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp834=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp834[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp835; _temp835.tag= Cyc_CfFlowInfo_InitsFL;
_temp835.f1= pinfo_dict; _temp835;}); _temp834;}); goto _LL837; _LL837: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp836); return( void*) _temp836;} _LL809: { struct Cyc_List_List*
_temp838= 0; goto _LL839; _LL839: Cyc_CfAbsexp_assign_escape(& _temp838,
_temp797, _temp799); for( 0; _temp838 != 0; _temp838=(( struct Cyc_List_List*)
_check_null( _temp838))->tl){ void* _temp840=( void*)(( struct Cyc_List_List*)
_check_null( _temp838))->hd; goto _LL841; _LL841: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp840)){ Cyc_Tcutil_terr( loc, _tag_arr("uninitialized value assigned to ambiguous place",
sizeof( unsigned char), 48u)); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( _temp840, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp795, _temp799);{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp844=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp842=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp842[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp843; _temp843.tag= Cyc_CfFlowInfo_InitsFL;
_temp843.f1= pinfo_dict; _temp843;}); _temp842;}); goto _LL845; _LL845: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp844); return( void*) _temp844;}} _LL803:;}}} _LL786:;}
_temp765= _temp761; goto _LL746; _LL746: { void* _temp846= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp765); goto _LL847; _LL847: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp846)){ void* _temp848= _temp765; struct Cyc_Absyn_Vardecl*
_temp854; _LL850: if(( unsigned int) _temp848 > 1u?*(( int*) _temp848) == Cyc_CfAbsexp_LocalOp:
0){ _LL855: _temp854=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp848)->f1; goto
_LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL851: if( Cyc_Tcutil_bits_only((
void*) _temp854->type)){ Cyc_Tcutil_warn( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp856=*(* _temp854->name).f2; xprintf("pointer-free %.*s may not be fully initialized",
_get_arr_size( _temp856, 1u), _temp856.curr);})); goto _LL849;} goto _LL853;
_LL853: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp857=
Cyc_CfAbsexp_absop2string( _temp765); xprintf("%.*s must be fully initialized to be used here",
_get_arr_size( _temp857, 1u), _temp857.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL849:;}{ struct Cyc_Dict_Dict* _temp858= Cyc_CfFlowInfo_escape_pointsto(
_temp846, pinfo_dict); goto _LL859; _LL859: if( _temp858 == pinfo_dict){ return
in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp862=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp860=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp860[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp861; _temp861.tag= Cyc_CfFlowInfo_InitsFL;
_temp861.f1= pinfo_dict; _temp861;}); _temp860;}); goto _LL863; _LL863: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp862); return( void*) _temp862;}}} _LL748: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp864=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp864[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp865; _temp865.tag= Cyc_CfFlowInfo_InitsFL;
_temp865.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( pinfo_dict, _temp769,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, _temp769)); _temp865;}); _temp864;});
_LL750: return( void*) Cyc_CfFlowInfo_BottomFL; _LL752: return in_flow; _LL754:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp771, in_flow); _LL756: for(
0; _temp773 != 0; _temp773=(( struct Cyc_List_List*) _check_null( _temp773))->tl){
in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp773))->hd, in_flow);} return in_flow; _LL758: { void* out_flow=(
void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp777 != 0; _temp777=(( struct Cyc_List_List*)
_check_null( _temp777))->tl){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp777))->hd, in_flow));}
return out_flow;} _LL760: { void* out_flow= in_flow; do { in_flow= out_flow;
out_flow= in_flow;{ struct Cyc_List_List* ael2= _temp781; for( 0; ael2 != 0;
ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( ael2))->hd, out_flow));}}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx(
out_flow, in_flow));{ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp866= in_flow;
struct Cyc_Dict_Dict* _temp872; _LL868: if( _temp866 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL869;} else{ goto _LL870;} _LL870: if(( unsigned int) _temp866 > 1u?*((
int*) _temp866) == Cyc_CfFlowInfo_InitsFL: 0){ _LL873: _temp872=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp866)->f1; goto _LL871;} else{ goto _LL867;} _LL869: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL871: pinfo_dict= _temp872; goto _LL867; _LL867:;}{ struct _RegionHandle
_temp874= _new_region(); struct _RegionHandle* spine_rgn=& _temp874;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp877; struct Cyc_List_List*
_temp879; struct _tuple5 _temp875= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL880: _temp879= _temp875.f1; goto _LL878; _LL878: _temp877= _temp875.f2;
goto _LL876; _LL876: for( 0; _temp877 != 0; _temp877=(( struct Cyc_List_List*)
_check_null( _temp877))->tl){ struct Cyc_CfFlowInfo_MallocPt_struct* _temp883=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp881=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp881[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp882; _temp882.tag= Cyc_CfFlowInfo_MallocPt;
_temp882.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp877))->hd; _temp882;}); _temp881;}); goto _LL884; _LL884: pinfo_dict=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(
pinfo_dict,( void*) _temp883,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp883));} out_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp885=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp885[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp886; _temp886.tag= Cyc_CfFlowInfo_InitsFL;
_temp886.f1= pinfo_dict; _temp886;}); _temp885;}); for( 0; _temp879 != 0;
_temp879=(( struct Cyc_List_List*) _check_null( _temp879))->tl){ out_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp879))->hd, out_flow);}};
_pop_region( spine_rgn);} return out_flow;}} _LL742:;}}