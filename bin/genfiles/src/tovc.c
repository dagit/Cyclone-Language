 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
env, struct Cyc_Absyn_Exp* e){ while( 1) { void* _temp0=( void*) e->r; struct
_tuple0* _temp48; struct _tuple0 _temp50; struct _tagged_arr* _temp51; void*
_temp53; struct Cyc_List_List* _temp55; struct Cyc_Absyn_Exp* _temp57; struct
Cyc_List_List* _temp59; struct Cyc_Absyn_Exp* _temp61; struct Cyc_Absyn_Exp*
_temp63; struct Cyc_Absyn_Exp* _temp65; struct Cyc_Absyn_Exp* _temp67; struct
Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Exp*
_temp73; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp* _temp77; struct
Cyc_Absyn_Exp* _temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
Cyc_Absyn_Exp* _temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Exp*
_temp93; struct Cyc_Absyn_Exp* _temp95; struct Cyc_List_List* _temp97; _LL2: if(*((
int*) _temp0) == Cyc_Absyn_Var_e){ _LL49: _temp48=(( struct Cyc_Absyn_Var_e_struct*)
_temp0)->f1; _temp50=* _temp48; _LL54: _temp53= _temp50.f1; if( _temp53 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL52;} else{ goto _LL4;} _LL52: _temp51= _temp50.f2;
goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) == Cyc_Absyn_Var_e){
goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp0) == Cyc_Absyn_Const_e){
goto _LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp0) == Cyc_Absyn_FnCall_e){
_LL58: _temp57=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f1; goto _LL56;
_LL56: _temp55=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f2; goto _LL9;}
else{ goto _LL10;} _LL10: if(*(( int*) _temp0) == Cyc_Absyn_Primop_e){ _LL60:
_temp59=(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f2; goto _LL11;} else{
goto _LL12;} _LL12: if(*(( int*) _temp0) == Cyc_Absyn_Conditional_e){ _LL66:
_temp65=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f1; goto _LL64;
_LL64: _temp63=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f2; goto
_LL62; _LL62: _temp61=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f3;
goto _LL13;} else{ goto _LL14;} _LL14: if(*(( int*) _temp0) == Cyc_Absyn_Subscript_e){
_LL70: _temp69=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f1; goto _LL68;
_LL68: _temp67=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(*(( int*) _temp0) == Cyc_Absyn_SeqExp_e){ _LL74:
_temp73=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f1; goto _LL72; _LL72:
_temp71=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f2; goto _LL17;} else{
goto _LL18;} _LL18: if(*(( int*) _temp0) == Cyc_Absyn_AssignOp_e){ _LL78:
_temp77=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f1; goto _LL76; _LL76:
_temp75=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f3; goto _LL19;} else{
goto _LL20;} _LL20: if(*(( int*) _temp0) == Cyc_Absyn_NoInstantiate_e){ _LL80:
_temp79=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp0)->f1; goto _LL21;}
else{ goto _LL22;} _LL22: if(*(( int*) _temp0) == Cyc_Absyn_Instantiate_e){
_LL82: _temp81=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f1; goto
_LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp0) == Cyc_Absyn_Cast_e){
_LL84: _temp83=(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f2; goto _LL25;}
else{ goto _LL26;} _LL26: if(*(( int*) _temp0) == Cyc_Absyn_Address_e){ _LL86:
_temp85=(( struct Cyc_Absyn_Address_e_struct*) _temp0)->f1; goto _LL27;} else{
goto _LL28;} _LL28: if(*(( int*) _temp0) == Cyc_Absyn_Sizeofexp_e){ _LL88:
_temp87=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp0)->f1; goto _LL29;} else{
goto _LL30;} _LL30: if(*(( int*) _temp0) == Cyc_Absyn_Deref_e){ _LL90: _temp89=((
struct Cyc_Absyn_Deref_e_struct*) _temp0)->f1; goto _LL31;} else{ goto _LL32;}
_LL32: if(*(( int*) _temp0) == Cyc_Absyn_StructMember_e){ _LL92: _temp91=((
struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f1; goto _LL33;} else{ goto
_LL34;} _LL34: if(*(( int*) _temp0) == Cyc_Absyn_StructArrow_e){ _LL94: _temp93=((
struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f1; goto _LL35;} else{ goto
_LL36;} _LL36: if(*(( int*) _temp0) == Cyc_Absyn_Increment_e){ _LL96: _temp95=((
struct Cyc_Absyn_Increment_e_struct*) _temp0)->f1; goto _LL37;} else{ goto _LL38;}
_LL38: if(*(( int*) _temp0) == Cyc_Absyn_Sizeoftyp_e){ goto _LL39;} else{ goto
_LL40;} _LL40: if(*(( int*) _temp0) == Cyc_Absyn_Offsetof_e){ goto _LL41;} else{
goto _LL42;} _LL42: if(*(( int*) _temp0) == Cyc_Absyn_Enum_e){ goto _LL43;}
else{ goto _LL44;} _LL44: if(*(( int*) _temp0) == Cyc_Absyn_UnresolvedMem_e){
_LL98: _temp97=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f2; goto
_LL45;} else{ goto _LL46;} _LL46: goto _LL47; _LL3: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)( env.varmap, _temp51)){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( env.varmap, _temp51))->r)); continue;} for(
0; env.boundvars != 0; env.boundvars=(( struct Cyc_List_List*) _check_null( env.boundvars))->tl){
if( Cyc_String_zstrptrcmp( _temp51,(*(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( env.boundvars))->hd)->name).f2) == 0){ return;}} for( 0; env.all_locals
!= 0; env.all_locals=(( struct Cyc_List_List*) _check_null( env.all_locals))->tl){
struct Cyc_Absyn_Vardecl* _temp99=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( env.all_locals))->hd; if( Cyc_String_zstrptrcmp( _temp51,(* _temp99->name).f2)
== 0){{ void* _temp100=( void*) _temp99->type; _LL102: if(( unsigned int)
_temp100 > 4u?*(( int*) _temp100) == Cyc_Absyn_ArrayType: 0){ goto _LL103;}
else{ goto _LL104;} _LL104: goto _LL105; _LL103: goto _LL101; _LL105:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp99->name, 0), 0))->r));
goto _LL101; _LL101:;}* env.freevars=(( struct Cyc_Set_Set*(*)( struct
_RegionHandle* rgn, struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt)) Cyc_Set_rinsert)(
env.rgn,* env.freevars, _temp99); return;}}( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp106=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp106[ 0]=({ struct Cyc_Core_Impossible_struct _temp107; _temp107.tag= Cyc_Core_Impossible;
_temp107.f1=( struct _tagged_arr)({ struct _tagged_arr _temp108=* _temp51;
xprintf("unbound variable %.*s found in box_free_vars_exp", _get_arr_size(
_temp108, 1u), _temp108.curr);}); _temp107;}); _temp106;})); _LL5: return; _LL7:
return; _LL9: Cyc_Tovc_box_free_vars_exp( env, _temp57); _temp59= _temp55; goto
_LL11; _LL11: for( 0; _temp59 != 0; _temp59=(( struct Cyc_List_List*)
_check_null( _temp59))->tl){ Cyc_Tovc_box_free_vars_exp( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp59))->hd);} return; _LL13: Cyc_Tovc_box_free_vars_exp(
env, _temp61); _temp69= _temp65; _temp67= _temp63; goto _LL15; _LL15: _temp73=
_temp69; _temp71= _temp67; goto _LL17; _LL17: _temp77= _temp73; _temp75= _temp71;
goto _LL19; _LL19: Cyc_Tovc_box_free_vars_exp( env, _temp75); _temp79= _temp77;
goto _LL21; _LL21: _temp81= _temp79; goto _LL23; _LL23: _temp83= _temp81; goto
_LL25; _LL25: _temp85= _temp83; goto _LL27; _LL27: _temp87= _temp85; goto _LL29;
_LL29: _temp89= _temp87; goto _LL31; _LL31: _temp91= _temp89; goto _LL33; _LL33:
_temp93= _temp91; goto _LL35; _LL35: _temp95= _temp93; goto _LL37; _LL37: Cyc_Tovc_box_free_vars_exp(
env, _temp95); return; _LL39: return; _LL41: return; _LL43: return; _LL45: for(
0; _temp97 != 0; _temp97=(( struct Cyc_List_List*) _check_null( _temp97))->tl){
Cyc_Tovc_box_free_vars_exp( env,(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp97))->hd)).f2);} return; _LL47:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp109=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp109[ 0]=({ struct
Cyc_Core_Impossible_struct _temp110; _temp110.tag= Cyc_Core_Impossible; _temp110.f1=
_tag_arr("bad exp form in Tocv::box_free_vars_exp", sizeof( unsigned char), 40u);
_temp110;}); _temp109;})); _LL1:;}} static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { void*
_temp111=( void*) s->r; struct Cyc_Absyn_Stmt* _temp143; struct Cyc_Absyn_Exp*
_temp145; struct Cyc_Absyn_Exp* _temp147; struct Cyc_Absyn_Stmt* _temp149;
struct Cyc_Absyn_Stmt* _temp151; struct Cyc_Absyn_Stmt* _temp153; struct Cyc_Absyn_Stmt*
_temp155; struct Cyc_Absyn_Exp* _temp157; struct Cyc_Absyn_Stmt* _temp159;
struct _tuple2 _temp161; struct Cyc_Absyn_Exp* _temp163; struct Cyc_Absyn_Stmt*
_temp165; struct _tuple2 _temp167; struct Cyc_Absyn_Exp* _temp169; struct
_tuple2 _temp171; struct Cyc_Absyn_Exp* _temp173; struct Cyc_Absyn_Exp* _temp175;
struct _tuple2 _temp177; struct Cyc_Absyn_Exp* _temp179; struct Cyc_Absyn_Stmt*
_temp181; struct Cyc_List_List* _temp183; struct Cyc_Absyn_Exp* _temp185; struct
Cyc_Absyn_Stmt* _temp187; struct Cyc_Absyn_Decl* _temp189; _LL113: if( _temp111
==( void*) Cyc_Absyn_Skip_s){ goto _LL114;} else{ goto _LL115;} _LL115: if((
unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Break_s: 0){ goto
_LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp111 > 1u?*(( int*)
_temp111) == Cyc_Absyn_Continue_s: 0){ goto _LL118;} else{ goto _LL119;} _LL119:
if(( unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Goto_s: 0){
goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int) _temp111 > 1u?*((
int*) _temp111) == Cyc_Absyn_Label_s: 0){ _LL144: _temp143=(( struct Cyc_Absyn_Label_s_struct*)
_temp111)->f2; goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Return_s: 0){ _LL146: _temp145=((
struct Cyc_Absyn_Return_s_struct*) _temp111)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Exp_s:
0){ _LL148: _temp147=(( struct Cyc_Absyn_Exp_s_struct*) _temp111)->f1; goto
_LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp111 > 1u?*(( int*)
_temp111) == Cyc_Absyn_Seq_s: 0){ _LL152: _temp151=(( struct Cyc_Absyn_Seq_s_struct*)
_temp111)->f1; goto _LL150; _LL150: _temp149=(( struct Cyc_Absyn_Seq_s_struct*)
_temp111)->f2; goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int)
_temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_IfThenElse_s: 0){ _LL158:
_temp157=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp111)->f1; goto _LL156;
_LL156: _temp155=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp111)->f2; goto
_LL154; _LL154: _temp153=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp111)->f3;
goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp111 > 1u?*((
int*) _temp111) == Cyc_Absyn_While_s: 0){ _LL162: _temp161=(( struct Cyc_Absyn_While_s_struct*)
_temp111)->f1; _LL164: _temp163= _temp161.f1; goto _LL160; _LL160: _temp159=((
struct Cyc_Absyn_While_s_struct*) _temp111)->f2; goto _LL132;} else{ goto _LL133;}
_LL133: if(( unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_For_s:
0){ _LL176: _temp175=(( struct Cyc_Absyn_For_s_struct*) _temp111)->f1; goto
_LL172; _LL172: _temp171=(( struct Cyc_Absyn_For_s_struct*) _temp111)->f2;
_LL174: _temp173= _temp171.f1; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_For_s_struct*)
_temp111)->f3; _LL170: _temp169= _temp167.f1; goto _LL166; _LL166: _temp165=((
struct Cyc_Absyn_For_s_struct*) _temp111)->f4; goto _LL134;} else{ goto _LL135;}
_LL135: if(( unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Do_s:
0){ _LL182: _temp181=(( struct Cyc_Absyn_Do_s_struct*) _temp111)->f1; goto
_LL178; _LL178: _temp177=(( struct Cyc_Absyn_Do_s_struct*) _temp111)->f2; _LL180:
_temp179= _temp177.f1; goto _LL136;} else{ goto _LL137;} _LL137: if((
unsigned int) _temp111 > 1u?*(( int*) _temp111) == Cyc_Absyn_Switch_s: 0){
_LL186: _temp185=(( struct Cyc_Absyn_Switch_s_struct*) _temp111)->f1; goto
_LL184; _LL184: _temp183=(( struct Cyc_Absyn_Switch_s_struct*) _temp111)->f2;
goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int) _temp111 > 1u?*((
int*) _temp111) == Cyc_Absyn_Decl_s: 0){ _LL190: _temp189=(( struct Cyc_Absyn_Decl_s_struct*)
_temp111)->f1; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_Decl_s_struct*)
_temp111)->f2; goto _LL140;} else{ goto _LL141;} _LL141: goto _LL142; _LL114:
return; _LL116: return; _LL118: return; _LL120: return; _LL122: s= _temp143;
continue; _LL124: if( _temp145 == 0){ return;} _temp147=( struct Cyc_Absyn_Exp*)
_check_null( _temp145); goto _LL126; _LL126: Cyc_Tovc_box_free_vars_exp( env,
_temp147); return; _LL128: Cyc_Tovc_box_free_vars_stmt( env, _temp151); s=
_temp149; continue; _LL130: Cyc_Tovc_box_free_vars_exp( env, _temp157); Cyc_Tovc_box_free_vars_stmt(
env, _temp155); s= _temp153; continue; _LL132: Cyc_Tovc_box_free_vars_exp( env,
_temp163); s= _temp159; continue; _LL134: Cyc_Tovc_box_free_vars_exp( env,
_temp175); Cyc_Tovc_box_free_vars_exp( env, _temp173); Cyc_Tovc_box_free_vars_exp(
env, _temp169); s= _temp165; continue; _LL136: Cyc_Tovc_box_free_vars_exp( env,
_temp179); s= _temp181; continue; _LL138: Cyc_Tovc_box_free_vars_exp( env,
_temp185); for( 0; _temp183 != 0; _temp183=(( struct Cyc_List_List*) _check_null(
_temp183))->tl){ Cyc_Tovc_box_free_vars_stmt( env,(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp183))->hd)->body);} return; _LL140:{
void* _temp191=( void*) _temp189->r; struct Cyc_Absyn_Vardecl* _temp197; _LL193:
if(*(( int*) _temp191) == Cyc_Absyn_Var_d){ _LL198: _temp197=(( struct Cyc_Absyn_Var_d_struct*)
_temp191)->f1; goto _LL194;} else{ goto _LL195;} _LL195: goto _LL196; _LL194:
env.boundvars=({ struct Cyc_List_List* _temp199=( struct Cyc_List_List*)
_region_malloc( env.rgn, sizeof( struct Cyc_List_List)); _temp199->hd=( void*)
_temp197; _temp199->tl= env.boundvars; _temp199;}); env.varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*, struct _tagged_arr*)) Cyc_Dict_rdelete_same)( env.varmap,(*
_temp197->name).f2); if( _temp197->initializer != 0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp197->initializer));} s= _temp187;
continue; _LL196: goto _LL192; _LL192:;} goto _LL142; _LL142:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp200=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp200[ 0]=({ struct
Cyc_Core_Impossible_struct _temp201; _temp201.tag= Cyc_Core_Impossible; _temp201.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp201;});
_temp200;})); _LL112:;}} struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* rgn, int(* comp)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*))) Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp202=({
struct Cyc_Tovc_BoxingEnv _temp221; _temp221.rgn= env.rgn; _temp221.all_locals=
env.all_locals; _temp221.varmap= env.varmap; _temp221.boundvars= 0; _temp221.freevars=&
freevars; _temp221;}); Cyc_Tovc_box_free_vars_stmt( _temp202, s);{ struct Cyc_List_List*
params= 0; struct Cyc_List_List* args= 0; struct Cyc_List_List* free_vars=((
struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)( freevars);
for( 0; free_vars != 0; free_vars=(( struct Cyc_List_List*) _check_null(
free_vars))->tl){ struct Cyc_Absyn_Vardecl* vd=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( free_vars))->hd; void* _temp203=( void*) vd->type;
void* _temp209; _LL205: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) ==
Cyc_Absyn_ArrayType: 0){ _LL210: _temp209=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp203)->f1; goto _LL206;} else{ goto _LL207;} _LL207: goto _LL208; _LL206:
params=({ struct Cyc_List_List* _temp211=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp211->hd=( void*)({ struct _tuple4* _temp212=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp212->f1=(* vd->name).f2;
_temp212->f2= vd->tq; _temp212->f3= Cyc_Absyn_cstar_typ( _temp209, Cyc_Absyn_empty_tqual());
_temp212;}); _temp211->tl= params; _temp211;}); args=({ struct Cyc_List_List*
_temp213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp213->hd=( void*) Cyc_Absyn_var_exp( vd->name, 0); _temp213->tl= args;
_temp213;}); goto _LL204; _LL208: params=({ struct Cyc_List_List* _temp214=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp214->hd=(
void*)({ struct _tuple4* _temp215=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp215->f1=(* vd->name).f2; _temp215->f2= vd->tq; _temp215->f3= Cyc_Absyn_cstar_typ((
void*) vd->type, Cyc_Absyn_empty_tqual()); _temp215;}); _temp214->tl= params;
_temp214;}); args=({ struct Cyc_List_List* _temp216=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp216->hd=( void*) Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp( vd->name, 0), 0); _temp216->tl= args; _temp216;}); goto
_LL204; _LL204:;}{ struct _tuple0* funname= Cyc_Toc_temp_var();* env.gen_ds=({
struct Cyc_List_List* _temp217=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp217->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp218=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp219; _temp219.tag= Cyc_Absyn_Fn_d; _temp219.f1=({ struct Cyc_Absyn_Fndecl*
_temp220=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp220->sc=( void*)(( void*) Cyc_Absyn_Static); _temp220->is_inline= 1;
_temp220->name= funname; _temp220->tvs= 0; _temp220->effect= 0; _temp220->ret_type=(
void*) rettype; _temp220->args= params; _temp220->c_varargs= 0; _temp220->cyc_varargs=
0; _temp220->rgn_po= 0; _temp220->body= s; _temp220->cached_typ= 0; _temp220->param_vardecls=
0; _temp220->attributes= 0; _temp220;}); _temp219;}); _temp218;}), 0); _temp217->tl=*
env.gen_ds; _temp217;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp222=( void*) e->r; struct _tuple0* _temp270;
struct _tuple0 _temp272; struct _tagged_arr* _temp273; void* _temp275; struct
Cyc_List_List* _temp277; struct Cyc_Absyn_Exp* _temp279; struct Cyc_List_List*
_temp281; struct Cyc_Absyn_Exp* _temp283; struct Cyc_Absyn_Exp* _temp285; struct
Cyc_Absyn_Exp* _temp287; struct Cyc_Absyn_Exp* _temp289; struct Cyc_Absyn_Exp*
_temp291; struct Cyc_Absyn_Exp* _temp293; struct Cyc_Absyn_Exp* _temp295; struct
Cyc_Absyn_Exp* _temp297; struct Cyc_Absyn_Exp* _temp299; struct Cyc_Absyn_Exp*
_temp301; struct Cyc_Absyn_Exp* _temp303; struct Cyc_Absyn_Exp* _temp305; struct
Cyc_Absyn_Exp* _temp307; struct Cyc_Absyn_Exp* _temp309; struct Cyc_Absyn_Exp*
_temp311; struct Cyc_Absyn_Exp* _temp313; struct Cyc_Absyn_Exp* _temp315; struct
Cyc_Absyn_Exp* _temp317; struct Cyc_List_List* _temp319; _LL224: if(*(( int*)
_temp222) == Cyc_Absyn_Var_e){ _LL271: _temp270=(( struct Cyc_Absyn_Var_e_struct*)
_temp222)->f1; _temp272=* _temp270; _LL276: _temp275= _temp272.f1; if( _temp275
==( void*) Cyc_Absyn_Loc_n){ goto _LL274;} else{ goto _LL226;} _LL274: _temp273=
_temp272.f2; goto _LL225;} else{ goto _LL226;} _LL226: if(*(( int*) _temp222) ==
Cyc_Absyn_Var_e){ goto _LL227;} else{ goto _LL228;} _LL228: if(*(( int*)
_temp222) == Cyc_Absyn_Const_e){ goto _LL229;} else{ goto _LL230;} _LL230: if(*((
int*) _temp222) == Cyc_Absyn_FnCall_e){ _LL280: _temp279=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp222)->f1; goto _LL278; _LL278: _temp277=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp222)->f2; goto _LL231;} else{ goto _LL232;} _LL232: if(*(( int*) _temp222)
== Cyc_Absyn_Primop_e){ _LL282: _temp281=(( struct Cyc_Absyn_Primop_e_struct*)
_temp222)->f2; goto _LL233;} else{ goto _LL234;} _LL234: if(*(( int*) _temp222)
== Cyc_Absyn_Conditional_e){ _LL288: _temp287=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp222)->f1; goto _LL286; _LL286: _temp285=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp222)->f2; goto _LL284; _LL284: _temp283=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp222)->f3; goto _LL235;} else{ goto _LL236;} _LL236: if(*(( int*) _temp222)
== Cyc_Absyn_Subscript_e){ _LL292: _temp291=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp222)->f1; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp222)->f2; goto _LL237;} else{ goto _LL238;} _LL238: if(*(( int*) _temp222)
== Cyc_Absyn_SeqExp_e){ _LL296: _temp295=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp222)->f1; goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp222)->f2; goto _LL239;} else{ goto _LL240;} _LL240: if(*(( int*) _temp222)
== Cyc_Absyn_AssignOp_e){ _LL300: _temp299=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp222)->f1; goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp222)->f3; goto _LL241;} else{ goto _LL242;} _LL242: if(*(( int*) _temp222)
== Cyc_Absyn_NoInstantiate_e){ _LL302: _temp301=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp222)->f1; goto _LL243;} else{ goto _LL244;} _LL244: if(*(( int*) _temp222)
== Cyc_Absyn_Instantiate_e){ _LL304: _temp303=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp222)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(*(( int*) _temp222)
== Cyc_Absyn_Cast_e){ _LL306: _temp305=(( struct Cyc_Absyn_Cast_e_struct*)
_temp222)->f2; goto _LL247;} else{ goto _LL248;} _LL248: if(*(( int*) _temp222)
== Cyc_Absyn_Address_e){ _LL308: _temp307=(( struct Cyc_Absyn_Address_e_struct*)
_temp222)->f1; goto _LL249;} else{ goto _LL250;} _LL250: if(*(( int*) _temp222)
== Cyc_Absyn_Sizeofexp_e){ _LL310: _temp309=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp222)->f1; goto _LL251;} else{ goto _LL252;} _LL252: if(*(( int*) _temp222)
== Cyc_Absyn_Deref_e){ _LL312: _temp311=(( struct Cyc_Absyn_Deref_e_struct*)
_temp222)->f1; goto _LL253;} else{ goto _LL254;} _LL254: if(*(( int*) _temp222)
== Cyc_Absyn_StructMember_e){ _LL314: _temp313=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp222)->f1; goto _LL255;} else{ goto _LL256;} _LL256: if(*(( int*) _temp222)
== Cyc_Absyn_StructArrow_e){ _LL316: _temp315=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp222)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(*(( int*) _temp222)
== Cyc_Absyn_Increment_e){ _LL318: _temp317=(( struct Cyc_Absyn_Increment_e_struct*)
_temp222)->f1; goto _LL259;} else{ goto _LL260;} _LL260: if(*(( int*) _temp222)
== Cyc_Absyn_Sizeoftyp_e){ goto _LL261;} else{ goto _LL262;} _LL262: if(*(( int*)
_temp222) == Cyc_Absyn_Offsetof_e){ goto _LL263;} else{ goto _LL264;} _LL264:
if(*(( int*) _temp222) == Cyc_Absyn_Enum_e){ goto _LL265;} else{ goto _LL266;}
_LL266: if(*(( int*) _temp222) == Cyc_Absyn_UnresolvedMem_e){ _LL320: _temp319=((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp222)->f2; goto _LL267;} else{
goto _LL268;} _LL268: goto _LL269; _LL225: if((( int(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_member)( varmap, _temp273)){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( varmap, _temp273))->r));} return; _LL227:
return; _LL229: return; _LL231: Cyc_Tovc_apply_varmap( varmap, _temp279);
_temp281= _temp277; goto _LL233; _LL233: for( 0; _temp281 != 0; _temp281=((
struct Cyc_List_List*) _check_null( _temp281))->tl){ Cyc_Tovc_apply_varmap(
varmap,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp281))->hd);}
return; _LL235: Cyc_Tovc_apply_varmap( varmap, _temp283); _temp291= _temp287;
_temp289= _temp285; goto _LL237; _LL237: _temp295= _temp291; _temp293= _temp289;
goto _LL239; _LL239: _temp299= _temp295; _temp297= _temp293; goto _LL241; _LL241:
Cyc_Tovc_apply_varmap( varmap, _temp297); _temp301= _temp299; goto _LL243;
_LL243: _temp303= _temp301; goto _LL245; _LL245: _temp305= _temp303; goto _LL247;
_LL247: _temp307= _temp305; goto _LL249; _LL249: _temp309= _temp307; goto _LL251;
_LL251: _temp311= _temp309; goto _LL253; _LL253: _temp313= _temp311; goto _LL255;
_LL255: _temp315= _temp313; goto _LL257; _LL257: _temp317= _temp315; goto _LL259;
_LL259: Cyc_Tovc_apply_varmap( varmap, _temp317); return; _LL261: return; _LL263:
return; _LL265: return; _LL267: for( 0; _temp319 != 0; _temp319=(( struct Cyc_List_List*)
_check_null( _temp319))->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp319))->hd)).f2);} return; _LL269:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp321=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp321[ 0]=({ struct
Cyc_Core_Impossible_struct _temp322; _temp322.tag= Cyc_Core_Impossible; _temp322.f1=
_tag_arr("bad exp form in Tocv::apply_varmap", sizeof( unsigned char), 35u);
_temp322;}); _temp321;})); _LL223:;} static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_exp(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){ void* _temp323=( void*)
s->r; struct Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Stmt* _temp339; struct
Cyc_Absyn_Stmt* _temp341; struct Cyc_Absyn_Stmt* _temp343; struct Cyc_Absyn_Stmt*
_temp345; struct Cyc_Absyn_Exp* _temp347; struct Cyc_Absyn_Stmt* _temp349;
struct Cyc_Absyn_Decl* _temp351; _LL325: if( _temp323 ==( void*) Cyc_Absyn_Skip_s){
goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int) _temp323 > 1u?*((
int*) _temp323) == Cyc_Absyn_Exp_s: 0){ _LL338: _temp337=(( struct Cyc_Absyn_Exp_s_struct*)
_temp323)->f1; goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int)
_temp323 > 1u?*(( int*) _temp323) == Cyc_Absyn_Seq_s: 0){ _LL342: _temp341=((
struct Cyc_Absyn_Seq_s_struct*) _temp323)->f1; goto _LL340; _LL340: _temp339=((
struct Cyc_Absyn_Seq_s_struct*) _temp323)->f2; goto _LL330;} else{ goto _LL331;}
_LL331: if(( unsigned int) _temp323 > 1u?*(( int*) _temp323) == Cyc_Absyn_IfThenElse_s:
0){ _LL348: _temp347=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp323)->f1;
goto _LL346; _LL346: _temp345=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp323)->f2;
goto _LL344; _LL344: _temp343=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp323)->f3;
goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int) _temp323 > 1u?*((
int*) _temp323) == Cyc_Absyn_Decl_s: 0){ _LL352: _temp351=(( struct Cyc_Absyn_Decl_s_struct*)
_temp323)->f1; goto _LL350; _LL350: _temp349=(( struct Cyc_Absyn_Decl_s_struct*)
_temp323)->f2; goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL326:
return Cyc_Absyn_true_exp( 0); _LL328: Cyc_Tovc_apply_varmap( env.varmap,
_temp337); return _temp337; _LL330: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp341), Cyc_Tovc_stmt_to_exp( env, _temp339), 0); _LL332: Cyc_Tovc_apply_varmap(
env.varmap, _temp347); return Cyc_Absyn_conditional_exp( _temp347, Cyc_Tovc_stmt_to_exp(
env, _temp345), Cyc_Tovc_stmt_to_exp( env, _temp343), 0); _LL334: { void*
_temp353=( void*) _temp351->r; struct Cyc_Absyn_Vardecl* _temp359; _LL355: if(*((
int*) _temp353) == Cyc_Absyn_Var_d){ _LL360: _temp359=(( struct Cyc_Absyn_Var_d_struct*)
_temp353)->f1; goto _LL356;} else{ goto _LL357;} _LL357: goto _LL358; _LL356: {
struct _tagged_arr* _temp361=(* _temp359->name).f2; struct _tuple0* _temp362=
Cyc_Toc_temp_var(); _temp359->name= _temp362; env.varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Exp* data))
Cyc_Dict_insert)( env.varmap, _temp361, Cyc_Absyn_var_exp( _temp362, 0)); if(
_temp359->initializer != 0){ Cyc_Tovc_apply_varmap( env.varmap,( struct Cyc_Absyn_Exp*)
_check_null( _temp359->initializer));{ void* _temp363=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp359->initializer))->r; _LL365: if(*(( int*) _temp363) == Cyc_Absyn_UnresolvedMem_e){
goto _LL366;} else{ goto _LL367;} _LL367: goto _LL368; _LL366: { struct _tuple0*
tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp359->type, _temp359->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp359->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp359->type);
goto _LL364;} _LL368: goto _LL364; _LL364:;}} env.all_locals=({ struct Cyc_List_List*
_temp369=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp369->hd=( void*) _temp359; _temp369->tl= env.all_locals; _temp369;});{
struct Cyc_Absyn_Exp* _temp370= _temp359->initializer; _temp359->initializer= 0;(
void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp351, Cyc_Absyn_new_stmt((
void*)( env.encloser)->r, 0), 0))->r));{ struct Cyc_Absyn_Exp* _temp371= Cyc_Tovc_stmt_to_exp(
env, _temp349); if( _temp370 != 0){ _temp371= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp359->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp370), 0), _temp371, 0);} return _temp371;}}} _LL358:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp372=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp372[ 0]=({ struct
Cyc_Core_Impossible_struct _temp373; _temp373.tag= Cyc_Core_Impossible; _temp373.f1=
_tag_arr("bad local decl in Tovc::stmt_to_exp", sizeof( unsigned char), 36u);
_temp373;}); _temp372;})); _LL354:;} _LL336: return Cyc_Tovc_stmt_to_fun( env, s,(
void*) Cyc_Absyn_VoidType); _LL324:;} static void Cyc_Tovc_stmt_to_vc( struct
Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s); static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Exp* e){ void* _temp374=( void*)
e->r; struct Cyc_List_List* _temp422; struct Cyc_Absyn_Exp* _temp424; struct Cyc_List_List*
_temp426; struct Cyc_Absyn_Exp* _temp428; struct Cyc_Absyn_Exp* _temp430; struct
Cyc_Absyn_Exp* _temp432; struct Cyc_Absyn_Exp* _temp434; struct Cyc_Absyn_Exp*
_temp436; struct Cyc_Absyn_Exp* _temp438; struct Cyc_Absyn_Exp* _temp440; struct
Cyc_Absyn_Exp* _temp442; struct Cyc_Absyn_Exp* _temp444; struct Cyc_Absyn_Exp*
_temp446; struct Cyc_Absyn_Exp* _temp448; struct Cyc_Absyn_Exp* _temp450; struct
Cyc_Absyn_Exp* _temp452; struct Cyc_Absyn_Exp* _temp454; struct Cyc_Absyn_Exp*
_temp456; struct Cyc_Absyn_Exp* _temp458; struct Cyc_Absyn_Exp* _temp460; struct
Cyc_Absyn_Exp* _temp462; struct Cyc_List_List* _temp464; struct Cyc_Absyn_Stmt*
_temp466; _LL376: if(*(( int*) _temp374) == Cyc_Absyn_Const_e){ goto _LL377;}
else{ goto _LL378;} _LL378: if(*(( int*) _temp374) == Cyc_Absyn_Var_e){ goto
_LL379;} else{ goto _LL380;} _LL380: if(*(( int*) _temp374) == Cyc_Absyn_FnCall_e){
_LL425: _temp424=(( struct Cyc_Absyn_FnCall_e_struct*) _temp374)->f1; goto
_LL423; _LL423: _temp422=(( struct Cyc_Absyn_FnCall_e_struct*) _temp374)->f2;
goto _LL381;} else{ goto _LL382;} _LL382: if(*(( int*) _temp374) == Cyc_Absyn_Primop_e){
_LL427: _temp426=(( struct Cyc_Absyn_Primop_e_struct*) _temp374)->f2; goto
_LL383;} else{ goto _LL384;} _LL384: if(*(( int*) _temp374) == Cyc_Absyn_Conditional_e){
_LL433: _temp432=(( struct Cyc_Absyn_Conditional_e_struct*) _temp374)->f1; goto
_LL431; _LL431: _temp430=(( struct Cyc_Absyn_Conditional_e_struct*) _temp374)->f2;
goto _LL429; _LL429: _temp428=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp374)->f3; goto _LL385;} else{ goto _LL386;} _LL386: if(*(( int*) _temp374)
== Cyc_Absyn_Subscript_e){ _LL437: _temp436=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp374)->f1; goto _LL435; _LL435: _temp434=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp374)->f2; goto _LL387;} else{ goto _LL388;} _LL388: if(*(( int*) _temp374)
== Cyc_Absyn_SeqExp_e){ _LL441: _temp440=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp374)->f1; goto _LL439; _LL439: _temp438=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp374)->f2; goto _LL389;} else{ goto _LL390;} _LL390: if(*(( int*) _temp374)
== Cyc_Absyn_AssignOp_e){ _LL445: _temp444=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp374)->f1; goto _LL443; _LL443: _temp442=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp374)->f3; goto _LL391;} else{ goto _LL392;} _LL392: if(*(( int*) _temp374)
== Cyc_Absyn_NoInstantiate_e){ _LL447: _temp446=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp374)->f1; goto _LL393;} else{ goto _LL394;} _LL394: if(*(( int*) _temp374)
== Cyc_Absyn_Instantiate_e){ _LL449: _temp448=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp374)->f1; goto _LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp374)
== Cyc_Absyn_Cast_e){ _LL451: _temp450=(( struct Cyc_Absyn_Cast_e_struct*)
_temp374)->f2; goto _LL397;} else{ goto _LL398;} _LL398: if(*(( int*) _temp374)
== Cyc_Absyn_Address_e){ _LL453: _temp452=(( struct Cyc_Absyn_Address_e_struct*)
_temp374)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp374)
== Cyc_Absyn_Sizeofexp_e){ _LL455: _temp454=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp374)->f1; goto _LL401;} else{ goto _LL402;} _LL402: if(*(( int*) _temp374)
== Cyc_Absyn_Deref_e){ _LL457: _temp456=(( struct Cyc_Absyn_Deref_e_struct*)
_temp374)->f1; goto _LL403;} else{ goto _LL404;} _LL404: if(*(( int*) _temp374)
== Cyc_Absyn_StructMember_e){ _LL459: _temp458=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp374)->f1; goto _LL405;} else{ goto _LL406;} _LL406: if(*(( int*) _temp374)
== Cyc_Absyn_StructArrow_e){ _LL461: _temp460=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp374)->f1; goto _LL407;} else{ goto _LL408;} _LL408: if(*(( int*) _temp374)
== Cyc_Absyn_Increment_e){ _LL463: _temp462=(( struct Cyc_Absyn_Increment_e_struct*)
_temp374)->f1; goto _LL409;} else{ goto _LL410;} _LL410: if(*(( int*) _temp374)
== Cyc_Absyn_Sizeoftyp_e){ goto _LL411;} else{ goto _LL412;} _LL412: if(*(( int*)
_temp374) == Cyc_Absyn_Offsetof_e){ goto _LL413;} else{ goto _LL414;} _LL414:
if(*(( int*) _temp374) == Cyc_Absyn_Enum_e){ goto _LL415;} else{ goto _LL416;}
_LL416: if(*(( int*) _temp374) == Cyc_Absyn_UnresolvedMem_e){ _LL465: _temp464=((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp374)->f2; goto _LL417;} else{
goto _LL418;} _LL418: if(*(( int*) _temp374) == Cyc_Absyn_StmtExp_e){ _LL467:
_temp466=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp374)->f1; goto _LL419;}
else{ goto _LL420;} _LL420: goto _LL421; _LL377: return; _LL379: return; _LL381:
Cyc_Tovc_exp_to_vc( env, _temp424); _temp426= _temp422; goto _LL383; _LL383:
for( 0; _temp426 != 0; _temp426=(( struct Cyc_List_List*) _check_null( _temp426))->tl){
Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp426))->hd);} return; _LL385: Cyc_Tovc_exp_to_vc( env, _temp428);
_temp436= _temp432; _temp434= _temp430; goto _LL387; _LL387: _temp440= _temp436;
_temp438= _temp434; goto _LL389; _LL389: _temp444= _temp440; _temp442= _temp438;
goto _LL391; _LL391: Cyc_Tovc_exp_to_vc( env, _temp442); _temp446= _temp444;
goto _LL393; _LL393: _temp448= _temp446; goto _LL395; _LL395: _temp450= _temp448;
goto _LL397; _LL397: _temp452= _temp450; goto _LL399; _LL399: _temp454= _temp452;
goto _LL401; _LL401: _temp456= _temp454; goto _LL403; _LL403: _temp458= _temp456;
goto _LL405; _LL405: _temp460= _temp458; goto _LL407; _LL407: _temp462= _temp460;
goto _LL409; _LL409: Cyc_Tovc_exp_to_vc( env, _temp462); return; _LL411: return;
_LL413: return; _LL415: return; _LL417: for( 0; _temp464 != 0; _temp464=((
struct Cyc_List_List*) _check_null( _temp464))->tl){ Cyc_Tovc_exp_to_vc( env,(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp464))->hd)).f2);}
return; _LL419: Cyc_Tovc_stmt_to_vc( env, _temp466);( void*)( e->r=( void*)((
void*)( Cyc_Tovc_stmt_to_exp( env, _temp466))->r)); return; _LL421:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp468=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp468[ 0]=({ struct
Cyc_Core_Impossible_struct _temp469; _temp469.tag= Cyc_Core_Impossible; _temp469.f1=
_tag_arr("bad exp form after xlation to C", sizeof( unsigned char), 32u);
_temp469;}); _temp468;})); _LL375:;} static void Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s){ while( 1) { env.encloser= s;{ void* _temp470=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp502; struct Cyc_Absyn_Exp* _temp504;
struct Cyc_Absyn_Exp* _temp506; struct Cyc_Absyn_Stmt* _temp508; struct Cyc_Absyn_Stmt*
_temp510; struct Cyc_Absyn_Stmt* _temp512; struct Cyc_Absyn_Stmt* _temp514;
struct Cyc_Absyn_Exp* _temp516; struct Cyc_Absyn_Stmt* _temp518; struct _tuple2
_temp520; struct Cyc_Absyn_Exp* _temp522; struct Cyc_Absyn_Stmt* _temp524;
struct _tuple2 _temp526; struct Cyc_Absyn_Exp* _temp528; struct _tuple2 _temp530;
struct Cyc_Absyn_Exp* _temp532; struct Cyc_Absyn_Exp* _temp534; struct _tuple2
_temp536; struct Cyc_Absyn_Exp* _temp538; struct Cyc_Absyn_Stmt* _temp540;
struct Cyc_List_List* _temp542; struct Cyc_Absyn_Exp* _temp544; struct Cyc_Absyn_Stmt*
_temp546; struct Cyc_Absyn_Decl* _temp548; _LL472: if( _temp470 ==( void*) Cyc_Absyn_Skip_s){
goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int) _temp470 > 1u?*((
int*) _temp470) == Cyc_Absyn_Break_s: 0){ goto _LL475;} else{ goto _LL476;}
_LL476: if(( unsigned int) _temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Continue_s:
0){ goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp470 > 1u?*((
int*) _temp470) == Cyc_Absyn_Goto_s: 0){ goto _LL479;} else{ goto _LL480;}
_LL480: if(( unsigned int) _temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Label_s:
0){ _LL503: _temp502=(( struct Cyc_Absyn_Label_s_struct*) _temp470)->f2; goto
_LL481;} else{ goto _LL482;} _LL482: if(( unsigned int) _temp470 > 1u?*(( int*)
_temp470) == Cyc_Absyn_Return_s: 0){ _LL505: _temp504=(( struct Cyc_Absyn_Return_s_struct*)
_temp470)->f1; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Exp_s: 0){ _LL507: _temp506=((
struct Cyc_Absyn_Exp_s_struct*) _temp470)->f1; goto _LL485;} else{ goto _LL486;}
_LL486: if(( unsigned int) _temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Seq_s:
0){ _LL511: _temp510=(( struct Cyc_Absyn_Seq_s_struct*) _temp470)->f1; goto
_LL509; _LL509: _temp508=(( struct Cyc_Absyn_Seq_s_struct*) _temp470)->f2; goto
_LL487;} else{ goto _LL488;} _LL488: if(( unsigned int) _temp470 > 1u?*(( int*)
_temp470) == Cyc_Absyn_IfThenElse_s: 0){ _LL517: _temp516=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp470)->f1; goto _LL515; _LL515: _temp514=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp470)->f2; goto _LL513; _LL513: _temp512=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp470)->f3; goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int)
_temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_While_s: 0){ _LL521: _temp520=((
struct Cyc_Absyn_While_s_struct*) _temp470)->f1; _LL523: _temp522= _temp520.f1;
goto _LL519; _LL519: _temp518=(( struct Cyc_Absyn_While_s_struct*) _temp470)->f2;
goto _LL491;} else{ goto _LL492;} _LL492: if(( unsigned int) _temp470 > 1u?*((
int*) _temp470) == Cyc_Absyn_For_s: 0){ _LL535: _temp534=(( struct Cyc_Absyn_For_s_struct*)
_temp470)->f1; goto _LL531; _LL531: _temp530=(( struct Cyc_Absyn_For_s_struct*)
_temp470)->f2; _LL533: _temp532= _temp530.f1; goto _LL527; _LL527: _temp526=((
struct Cyc_Absyn_For_s_struct*) _temp470)->f3; _LL529: _temp528= _temp526.f1;
goto _LL525; _LL525: _temp524=(( struct Cyc_Absyn_For_s_struct*) _temp470)->f4;
goto _LL493;} else{ goto _LL494;} _LL494: if(( unsigned int) _temp470 > 1u?*((
int*) _temp470) == Cyc_Absyn_Do_s: 0){ _LL541: _temp540=(( struct Cyc_Absyn_Do_s_struct*)
_temp470)->f1; goto _LL537; _LL537: _temp536=(( struct Cyc_Absyn_Do_s_struct*)
_temp470)->f2; _LL539: _temp538= _temp536.f1; goto _LL495;} else{ goto _LL496;}
_LL496: if(( unsigned int) _temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Switch_s:
0){ _LL545: _temp544=(( struct Cyc_Absyn_Switch_s_struct*) _temp470)->f1; goto
_LL543; _LL543: _temp542=(( struct Cyc_Absyn_Switch_s_struct*) _temp470)->f2;
goto _LL497;} else{ goto _LL498;} _LL498: if(( unsigned int) _temp470 > 1u?*((
int*) _temp470) == Cyc_Absyn_Decl_s: 0){ _LL549: _temp548=(( struct Cyc_Absyn_Decl_s_struct*)
_temp470)->f1; goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_Decl_s_struct*)
_temp470)->f2; goto _LL499;} else{ goto _LL500;} _LL500: goto _LL501; _LL473:
return; _LL475: return; _LL477: return; _LL479: return; _LL481: s= _temp502;
continue; _LL483: if( _temp504 == 0){ return;} _temp506=( struct Cyc_Absyn_Exp*)
_check_null( _temp504); goto _LL485; _LL485: Cyc_Tovc_exp_to_vc( env, _temp506);
return; _LL487: Cyc_Tovc_stmt_to_vc( env, _temp510); s= _temp508; continue;
_LL489: Cyc_Tovc_exp_to_vc( env, _temp516); Cyc_Tovc_stmt_to_vc( env, _temp514);
s= _temp512; continue; _LL491: Cyc_Tovc_exp_to_vc( env, _temp522); s= _temp518;
continue; _LL493: Cyc_Tovc_exp_to_vc( env, _temp534); Cyc_Tovc_exp_to_vc( env,
_temp532); Cyc_Tovc_exp_to_vc( env, _temp528); s= _temp524; continue; _LL495:
Cyc_Tovc_exp_to_vc( env, _temp538); s= _temp540; continue; _LL497: Cyc_Tovc_exp_to_vc(
env, _temp544); for( 0; _temp542 != 0; _temp542=(( struct Cyc_List_List*)
_check_null( _temp542))->tl){ Cyc_Tovc_stmt_to_vc( env,(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp542))->hd)->body);} return; _LL499:{
void* _temp550=( void*) _temp548->r; struct Cyc_Absyn_Vardecl* _temp556; _LL552:
if(*(( int*) _temp550) == Cyc_Absyn_Var_d){ _LL557: _temp556=(( struct Cyc_Absyn_Var_d_struct*)
_temp550)->f1; goto _LL553;} else{ goto _LL554;} _LL554: goto _LL555; _LL553:{
void* _temp558= Cyc_Tcutil_compress(( void*) _temp556->type); _LL560: if((
unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_ArrayType: 0){ goto
_LL561;} else{ goto _LL562;} _LL562: goto _LL563; _LL561: if( _temp556->initializer
!= 0){ void* _temp564=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp556->initializer))->r;
struct Cyc_List_List* _temp570; _LL566: if(*(( int*) _temp564) == Cyc_Absyn_UnresolvedMem_e){
_LL571: _temp570=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp564)->f2;
goto _LL567;} else{ goto _LL568;} _LL568: goto _LL569; _LL567: _temp556->initializer=
0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp( _temp556->name, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_new_stmt(( void*) _temp546->r, 0);{ unsigned int i= 0; for( 0;
_temp570 != 0;( _temp570=(( struct Cyc_List_List*) _check_null( _temp570))->tl,
++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp(
arr, Cyc_Absyn_uint_exp( i, 0), 0),(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp570))->hd)).f2, 0), 0), s3, 0);}}( void*)( _temp546->r=( void*)((
void*) s3->r)); goto _LL565;} _LL569:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp572=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp572[ 0]=({ struct Cyc_Core_Impossible_struct _temp573; _temp573.tag= Cyc_Core_Impossible;
_temp573.f1= _tag_arr("array type bad initializer", sizeof( unsigned char), 27u);
_temp573;}); _temp572;})); _LL565:;} goto _LL559; _LL563: goto _LL559; _LL559:;}
env.all_locals=({ struct Cyc_List_List* _temp574=( struct Cyc_List_List*)
_region_malloc( env.rgn, sizeof( struct Cyc_List_List)); _temp574->hd=( void*)
_temp556; _temp574->tl= env.all_locals; _temp574;}); if( _temp556->initializer
!= 0){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*) _check_null( _temp556->initializer));}
s= _temp546; continue; _LL555: goto _LL551; _LL551:;} goto _LL501; _LL501:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp575=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp575[ 0]=({ struct
Cyc_Core_Impossible_struct _temp576; _temp576.tag= Cyc_Core_Impossible; _temp576.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp576;});
_temp575;})); _LL471:;}}} struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List*
old_ds){ struct Cyc_List_List* _temp577= 0; for( 0; old_ds != 0; old_ds=((
struct Cyc_List_List*) _check_null( old_ds))->tl){ struct Cyc_Absyn_Decl*
_temp578=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( old_ds))->hd;
void* _temp579=( void*) _temp578->r; struct Cyc_Absyn_Fndecl* _temp605; struct
Cyc_List_List* _temp607; struct Cyc_List_List* _temp609; struct Cyc_List_List*
_temp611; _LL581: if(*(( int*) _temp579) == Cyc_Absyn_Fn_d){ _LL606: _temp605=((
struct Cyc_Absyn_Fn_d_struct*) _temp579)->f1; goto _LL582;} else{ goto _LL583;}
_LL583: if(*(( int*) _temp579) == Cyc_Absyn_Var_d){ goto _LL584;} else{ goto
_LL585;} _LL585: if(*(( int*) _temp579) == Cyc_Absyn_Let_d){ goto _LL586;} else{
goto _LL587;} _LL587: if(*(( int*) _temp579) == Cyc_Absyn_Letv_d){ goto _LL588;}
else{ goto _LL589;} _LL589: if(*(( int*) _temp579) == Cyc_Absyn_Struct_d){ goto
_LL590;} else{ goto _LL591;} _LL591: if(*(( int*) _temp579) == Cyc_Absyn_Union_d){
goto _LL592;} else{ goto _LL593;} _LL593: if(*(( int*) _temp579) == Cyc_Absyn_Tunion_d){
goto _LL594;} else{ goto _LL595;} _LL595: if(*(( int*) _temp579) == Cyc_Absyn_Enum_d){
goto _LL596;} else{ goto _LL597;} _LL597: if(*(( int*) _temp579) == Cyc_Absyn_Typedef_d){
goto _LL598;} else{ goto _LL599;} _LL599: if(*(( int*) _temp579) == Cyc_Absyn_Namespace_d){
_LL608: _temp607=(( struct Cyc_Absyn_Namespace_d_struct*) _temp579)->f2; goto
_LL600;} else{ goto _LL601;} _LL601: if(*(( int*) _temp579) == Cyc_Absyn_Using_d){
_LL610: _temp609=(( struct Cyc_Absyn_Using_d_struct*) _temp579)->f2; goto _LL602;}
else{ goto _LL603;} _LL603: if(*(( int*) _temp579) == Cyc_Absyn_ExternC_d){
_LL612: _temp611=(( struct Cyc_Absyn_ExternC_d_struct*) _temp579)->f1; goto
_LL604;} else{ goto _LL580;} _LL582:{ struct _RegionHandle _temp613= _new_region();
struct _RegionHandle* rgn=& _temp613; _push_region( rgn);{ struct Cyc_List_List**
_temp614=({ struct Cyc_List_List** _temp616=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp616[ 0]= 0; _temp616;});
Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp615; _temp615.rgn= rgn;
_temp615.all_locals=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
Cyc_List_List* src)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp605->param_vardecls))->v); _temp615.varmap=(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*, int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_rempty)( rgn, Cyc_String_zstrptrcmp); _temp615.encloser= _temp605->body;
_temp615.gen_ds= _temp614; _temp615;}), _temp605->body); _temp577=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(* _temp614,
_temp577);}; _pop_region( rgn);} goto _LL584; _LL584: goto _LL586; _LL586: goto
_LL588; _LL588: goto _LL590; _LL590: goto _LL592; _LL592: goto _LL594; _LL594:
goto _LL596; _LL596: goto _LL598; _LL598: _temp577=({ struct Cyc_List_List*
_temp617=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp617->hd=( void*) _temp578; _temp617->tl= _temp577; _temp617;}); goto _LL580;
_LL600: _temp609= _temp607; goto _LL602; _LL602: _temp611= _temp609; goto _LL604;
_LL604:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp618=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp618[ 0]=({ struct Cyc_Core_Impossible_struct _temp619; _temp619.tag= Cyc_Core_Impossible;
_temp619.f1= _tag_arr("nested translation unit after translation to C", sizeof(
unsigned char), 47u); _temp619;}); _temp618;})); _LL580:;} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp577);}