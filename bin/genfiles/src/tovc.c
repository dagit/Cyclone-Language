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
_check_null( env.all_locals))->hd; if( Cyc_String_zstrptrcmp( _temp53,(*
_temp137->name).f2) == 0){{ void* _temp138=( void*) _temp137->type; struct Cyc_Absyn_Exp*
_temp144; struct Cyc_Absyn_Tqual _temp146; void* _temp148; _LL140: if((
unsigned int) _temp138 > 4u?*(( int*) _temp138) == Cyc_Absyn_ArrayType: 0){
_LL149: _temp148=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp138)->f1;
goto _LL147; _LL147: _temp146=(( struct Cyc_Absyn_ArrayType_struct*) _temp138)->f2;
goto _LL145; _LL145: _temp144=(( struct Cyc_Absyn_ArrayType_struct*) _temp138)->f3;
goto _LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL141: goto _LL139;
_LL143:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp137->name, 0), 0))->r)); goto _LL139; _LL139:;}* env.freevars=(( struct Cyc_Set_Set*(*)(
struct _RegionHandle* rgn, struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt))
Cyc_Set_rinsert)( env.rgn,* env.freevars, _temp137); return;}}( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp150=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp150[ 0]=({ struct
Cyc_Core_Impossible_struct _temp151; _temp151.tag= Cyc_Core_Impossible; _temp151.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp152=* _temp53; xprintf("unbound variable %.*s found in box_free_vars_exp",
_get_arr_size( _temp152, 1u), _temp152.curr);}); _temp151;}); _temp150;})); _LL5:
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
_temp153=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp153[ 0]=({ struct Cyc_Core_Impossible_struct _temp154; _temp154.tag= Cyc_Core_Impossible;
_temp154.f1= _tag_arr("bad exp form in Tocv::box_free_vars_exp", sizeof(
unsigned char), 40u); _temp154;}); _temp153;})); _LL1:;}} static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { void*
_temp155=( void*) s->r; struct Cyc_Absyn_Stmt* _temp187; struct Cyc_Absyn_Stmt*
_temp189; struct Cyc_Absyn_Stmt* _temp191; struct _tagged_arr* _temp193; struct
Cyc_Absyn_Stmt* _temp195; struct _tagged_arr* _temp197; struct Cyc_Absyn_Exp*
_temp199; struct Cyc_Absyn_Exp* _temp201; struct Cyc_Absyn_Stmt* _temp203;
struct Cyc_Absyn_Stmt* _temp205; struct Cyc_Absyn_Stmt* _temp207; struct Cyc_Absyn_Stmt*
_temp209; struct Cyc_Absyn_Exp* _temp211; struct Cyc_Absyn_Stmt* _temp213;
struct _tuple2 _temp215; struct Cyc_Absyn_Stmt* _temp217; struct Cyc_Absyn_Exp*
_temp219; struct Cyc_Absyn_Stmt* _temp221; struct _tuple2 _temp223; struct Cyc_Absyn_Stmt*
_temp225; struct Cyc_Absyn_Exp* _temp227; struct _tuple2 _temp229; struct Cyc_Absyn_Stmt*
_temp231; struct Cyc_Absyn_Exp* _temp233; struct Cyc_Absyn_Exp* _temp235; struct
_tuple2 _temp237; struct Cyc_Absyn_Stmt* _temp239; struct Cyc_Absyn_Exp*
_temp241; struct Cyc_Absyn_Stmt* _temp243; struct Cyc_List_List* _temp245;
struct Cyc_Absyn_Exp* _temp247; struct Cyc_Absyn_Stmt* _temp249; struct Cyc_Absyn_Decl*
_temp251; _LL157: if( _temp155 ==( void*) Cyc_Absyn_Skip_s){ goto _LL158;} else{
goto _LL159;} _LL159: if(( unsigned int) _temp155 > 1u?*(( int*) _temp155) ==
Cyc_Absyn_Break_s: 0){ _LL188: _temp187=(( struct Cyc_Absyn_Break_s_struct*)
_temp155)->f1; goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Continue_s: 0){ _LL190: _temp189=((
struct Cyc_Absyn_Continue_s_struct*) _temp155)->f1; goto _LL162;} else{ goto
_LL163;} _LL163: if(( unsigned int) _temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Goto_s:
0){ _LL194: _temp193=(( struct Cyc_Absyn_Goto_s_struct*) _temp155)->f1; goto
_LL192; _LL192: _temp191=(( struct Cyc_Absyn_Goto_s_struct*) _temp155)->f2; goto
_LL164;} else{ goto _LL165;} _LL165: if(( unsigned int) _temp155 > 1u?*(( int*)
_temp155) == Cyc_Absyn_Label_s: 0){ _LL198: _temp197=(( struct Cyc_Absyn_Label_s_struct*)
_temp155)->f1; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_Label_s_struct*)
_temp155)->f2; goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Return_s: 0){ _LL200: _temp199=((
struct Cyc_Absyn_Return_s_struct*) _temp155)->f1; goto _LL168;} else{ goto
_LL169;} _LL169: if(( unsigned int) _temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Exp_s:
0){ _LL202: _temp201=(( struct Cyc_Absyn_Exp_s_struct*) _temp155)->f1; goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp155 > 1u?*(( int*)
_temp155) == Cyc_Absyn_Seq_s: 0){ _LL206: _temp205=(( struct Cyc_Absyn_Seq_s_struct*)
_temp155)->f1; goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Seq_s_struct*)
_temp155)->f2; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_IfThenElse_s: 0){ _LL212:
_temp211=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp155)->f1; goto _LL210;
_LL210: _temp209=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp155)->f2; goto
_LL208; _LL208: _temp207=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp155)->f3;
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp155 > 1u?*((
int*) _temp155) == Cyc_Absyn_While_s: 0){ _LL216: _temp215=(( struct Cyc_Absyn_While_s_struct*)
_temp155)->f1; _LL220: _temp219= _temp215.f1; goto _LL218; _LL218: _temp217=
_temp215.f2; goto _LL214; _LL214: _temp213=(( struct Cyc_Absyn_While_s_struct*)
_temp155)->f2; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_For_s: 0){ _LL236: _temp235=((
struct Cyc_Absyn_For_s_struct*) _temp155)->f1; goto _LL230; _LL230: _temp229=((
struct Cyc_Absyn_For_s_struct*) _temp155)->f2; _LL234: _temp233= _temp229.f1;
goto _LL232; _LL232: _temp231= _temp229.f2; goto _LL224; _LL224: _temp223=((
struct Cyc_Absyn_For_s_struct*) _temp155)->f3; _LL228: _temp227= _temp223.f1;
goto _LL226; _LL226: _temp225= _temp223.f2; goto _LL222; _LL222: _temp221=((
struct Cyc_Absyn_For_s_struct*) _temp155)->f4; goto _LL178;} else{ goto _LL179;}
_LL179: if(( unsigned int) _temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Do_s:
0){ _LL244: _temp243=(( struct Cyc_Absyn_Do_s_struct*) _temp155)->f1; goto
_LL238; _LL238: _temp237=(( struct Cyc_Absyn_Do_s_struct*) _temp155)->f2; _LL242:
_temp241= _temp237.f1; goto _LL240; _LL240: _temp239= _temp237.f2; goto _LL180;}
else{ goto _LL181;} _LL181: if(( unsigned int) _temp155 > 1u?*(( int*) _temp155)
== Cyc_Absyn_Switch_s: 0){ _LL248: _temp247=(( struct Cyc_Absyn_Switch_s_struct*)
_temp155)->f1; goto _LL246; _LL246: _temp245=(( struct Cyc_Absyn_Switch_s_struct*)
_temp155)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Decl_s: 0){ _LL252: _temp251=((
struct Cyc_Absyn_Decl_s_struct*) _temp155)->f1; goto _LL250; _LL250: _temp249=((
struct Cyc_Absyn_Decl_s_struct*) _temp155)->f2; goto _LL184;} else{ goto _LL185;}
_LL185: goto _LL186; _LL158: return; _LL160: return; _LL162: return; _LL164:
return; _LL166: s= _temp195; continue; _LL168: if( _temp199 == 0){ return;}
_temp201=( struct Cyc_Absyn_Exp*) _check_null( _temp199); goto _LL170; _LL170:
Cyc_Tovc_box_free_vars_exp( env, _temp201); return; _LL172: Cyc_Tovc_box_free_vars_stmt(
env, _temp205); s= _temp203; continue; _LL174: Cyc_Tovc_box_free_vars_exp( env,
_temp211); Cyc_Tovc_box_free_vars_stmt( env, _temp209); s= _temp207; continue;
_LL176: Cyc_Tovc_box_free_vars_exp( env, _temp219); s= _temp213; continue;
_LL178: Cyc_Tovc_box_free_vars_exp( env, _temp235); Cyc_Tovc_box_free_vars_exp(
env, _temp233); Cyc_Tovc_box_free_vars_exp( env, _temp227); s= _temp221;
continue; _LL180: Cyc_Tovc_box_free_vars_exp( env, _temp241); s= _temp243;
continue; _LL182: Cyc_Tovc_box_free_vars_exp( env, _temp247); for( 0; _temp245
!= 0; _temp245=(( struct Cyc_List_List*) _check_null( _temp245))->tl){ Cyc_Tovc_box_free_vars_stmt(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp245))->hd)->body);} return; _LL184:{ void* _temp253=( void*) _temp251->r;
struct Cyc_Absyn_Vardecl* _temp259; _LL255: if(*(( int*) _temp253) == Cyc_Absyn_Var_d){
_LL260: _temp259=(( struct Cyc_Absyn_Var_d_struct*) _temp253)->f1; goto _LL256;}
else{ goto _LL257;} _LL257: goto _LL258; _LL256: env.boundvars=({ struct Cyc_List_List*
_temp261=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp261->hd=( void*) _temp259; _temp261->tl= env.boundvars; _temp261;}); env.varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*, struct _tagged_arr*)) Cyc_Dict_rdelete_same)(
env.varmap,(* _temp259->name).f2); if( _temp259->initializer != 0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp259->initializer));} s= _temp249;
continue; _LL258: goto _LL254; _LL254:;} goto _LL186; _LL186:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp262=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp262[ 0]=({ struct
Cyc_Core_Impossible_struct _temp263; _temp263.tag= Cyc_Core_Impossible; _temp263.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp263;});
_temp262;})); _LL156:;}} struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* rgn, int(* comp)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*))) Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp264=({
struct Cyc_Tovc_BoxingEnv _temp287; _temp287.rgn= env.rgn; _temp287.all_locals=
env.all_locals; _temp287.varmap= env.varmap; _temp287.boundvars= 0; _temp287.freevars=&
freevars; _temp287;}); Cyc_Tovc_box_free_vars_stmt( _temp264, s);{ struct Cyc_List_List*
params= 0; struct Cyc_List_List* args= 0; struct Cyc_List_List* free_vars=((
struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)( freevars);
for( 0; free_vars != 0; free_vars=(( struct Cyc_List_List*) _check_null(
free_vars))->tl){ struct Cyc_Absyn_Vardecl* vd=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( free_vars))->hd; void* _temp265=( void*) vd->type;
struct Cyc_Absyn_Exp* _temp271; struct Cyc_Absyn_Tqual _temp273; void* _temp275;
_LL267: if(( unsigned int) _temp265 > 4u?*(( int*) _temp265) == Cyc_Absyn_ArrayType:
0){ _LL276: _temp275=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp265)->f1;
goto _LL274; _LL274: _temp273=(( struct Cyc_Absyn_ArrayType_struct*) _temp265)->f2;
goto _LL272; _LL272: _temp271=(( struct Cyc_Absyn_ArrayType_struct*) _temp265)->f3;
goto _LL268;} else{ goto _LL269;} _LL269: goto _LL270; _LL268: params=({ struct
Cyc_List_List* _temp277=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp277->hd=( void*)({ struct _tuple4* _temp278=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp278->f1=(* vd->name).f2; _temp278->f2= vd->tq;
_temp278->f3= Cyc_Absyn_cstar_typ( _temp275, Cyc_Absyn_empty_tqual()); _temp278;});
_temp277->tl= params; _temp277;}); args=({ struct Cyc_List_List* _temp279=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp279->hd=(
void*) Cyc_Absyn_var_exp( vd->name, 0); _temp279->tl= args; _temp279;}); goto
_LL266; _LL270: params=({ struct Cyc_List_List* _temp280=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp280->hd=( void*)({ struct
_tuple4* _temp281=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp281->f1=(* vd->name).f2; _temp281->f2= vd->tq; _temp281->f3= Cyc_Absyn_cstar_typ((
void*) vd->type, Cyc_Absyn_empty_tqual()); _temp281;}); _temp280->tl= params;
_temp280;}); args=({ struct Cyc_List_List* _temp282=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp282->hd=( void*) Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp( vd->name, 0), 0); _temp282->tl= args; _temp282;}); goto
_LL266; _LL266:;}{ struct _tuple0* funname= Cyc_Toc_temp_var();* env.gen_ds=({
struct Cyc_List_List* _temp283=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp283->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp284=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp284[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp285; _temp285.tag= Cyc_Absyn_Fn_d; _temp285.f1=({ struct Cyc_Absyn_Fndecl*
_temp286=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp286->sc=( void*)(( void*) Cyc_Absyn_Static); _temp286->is_inline= 1;
_temp286->name= funname; _temp286->tvs= 0; _temp286->effect= 0; _temp286->ret_type=(
void*) rettype; _temp286->args= params; _temp286->c_varargs= 0; _temp286->cyc_varargs=
0; _temp286->rgn_po= 0; _temp286->body= s; _temp286->cached_typ= 0; _temp286->param_vardecls=
0; _temp286->attributes= 0; _temp286;}); _temp285;}); _temp284;}), 0); _temp283->tl=*
env.gen_ds; _temp283;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp288=( void*) e->r; void* _temp336; struct
_tuple0* _temp338; struct _tuple0 _temp340; struct _tagged_arr* _temp341; void*
_temp343; void* _temp345; struct _tuple0* _temp347; void* _temp349; struct Cyc_Absyn_VarargCallInfo*
_temp351; struct Cyc_List_List* _temp353; struct Cyc_Absyn_Exp* _temp355; struct
Cyc_List_List* _temp357; void* _temp359; struct Cyc_Absyn_Exp* _temp361; struct
Cyc_Absyn_Exp* _temp363; struct Cyc_Absyn_Exp* _temp365; struct Cyc_Absyn_Exp*
_temp367; struct Cyc_Absyn_Exp* _temp369; struct Cyc_Absyn_Exp* _temp371; struct
Cyc_Absyn_Exp* _temp373; struct Cyc_Absyn_Exp* _temp375; struct Cyc_Core_Opt*
_temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_Absyn_Exp* _temp381; struct
Cyc_List_List* _temp383; struct Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Exp*
_temp387; void* _temp389; struct Cyc_Absyn_Exp* _temp391; struct Cyc_Absyn_Exp*
_temp393; struct Cyc_Absyn_Exp* _temp395; struct _tagged_arr* _temp397; struct
Cyc_Absyn_Exp* _temp399; struct _tagged_arr* _temp401; struct Cyc_Absyn_Exp*
_temp403; void* _temp405; struct Cyc_Absyn_Exp* _temp407; void* _temp409; struct
_tagged_arr* _temp411; void* _temp413; struct Cyc_Absyn_Enumfield* _temp415;
struct Cyc_Absyn_Enumdecl* _temp417; struct _tuple0* _temp419; struct Cyc_List_List*
_temp421; struct Cyc_Core_Opt* _temp423; _LL290: if(*(( int*) _temp288) == Cyc_Absyn_Var_e){
_LL339: _temp338=(( struct Cyc_Absyn_Var_e_struct*) _temp288)->f1; _temp340=*
_temp338; _LL344: _temp343= _temp340.f1; if( _temp343 ==( void*) Cyc_Absyn_Loc_n){
goto _LL342;} else{ goto _LL292;} _LL342: _temp341= _temp340.f2; goto _LL337;
_LL337: _temp336=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp288)->f2; goto
_LL291;} else{ goto _LL292;} _LL292: if(*(( int*) _temp288) == Cyc_Absyn_Var_e){
_LL348: _temp347=(( struct Cyc_Absyn_Var_e_struct*) _temp288)->f1; goto _LL346;
_LL346: _temp345=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp288)->f2; goto
_LL293;} else{ goto _LL294;} _LL294: if(*(( int*) _temp288) == Cyc_Absyn_Const_e){
_LL350: _temp349=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp288)->f1;
goto _LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp288) == Cyc_Absyn_FnCall_e){
_LL356: _temp355=(( struct Cyc_Absyn_FnCall_e_struct*) _temp288)->f1; goto
_LL354; _LL354: _temp353=(( struct Cyc_Absyn_FnCall_e_struct*) _temp288)->f2;
goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_FnCall_e_struct*) _temp288)->f3;
goto _LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp288) == Cyc_Absyn_Primop_e){
_LL360: _temp359=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp288)->f1;
goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_Primop_e_struct*) _temp288)->f2;
goto _LL299;} else{ goto _LL300;} _LL300: if(*(( int*) _temp288) == Cyc_Absyn_Conditional_e){
_LL366: _temp365=(( struct Cyc_Absyn_Conditional_e_struct*) _temp288)->f1; goto
_LL364; _LL364: _temp363=(( struct Cyc_Absyn_Conditional_e_struct*) _temp288)->f2;
goto _LL362; _LL362: _temp361=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp288)->f3; goto _LL301;} else{ goto _LL302;} _LL302: if(*(( int*) _temp288)
== Cyc_Absyn_Subscript_e){ _LL370: _temp369=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp288)->f1; goto _LL368; _LL368: _temp367=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp288)->f2; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*) _temp288)
== Cyc_Absyn_SeqExp_e){ _LL374: _temp373=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp288)->f1; goto _LL372; _LL372: _temp371=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp288)->f2; goto _LL305;} else{ goto _LL306;} _LL306: if(*(( int*) _temp288)
== Cyc_Absyn_AssignOp_e){ _LL380: _temp379=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp288)->f1; goto _LL378; _LL378: _temp377=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp288)->f2; goto _LL376; _LL376: _temp375=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp288)->f3; goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp288)
== Cyc_Absyn_NoInstantiate_e){ _LL382: _temp381=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp288)->f1; goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp288)
== Cyc_Absyn_Instantiate_e){ _LL386: _temp385=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp288)->f1; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp288)->f2; goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp288)
== Cyc_Absyn_Cast_e){ _LL390: _temp389=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp288)->f1; goto _LL388; _LL388: _temp387=(( struct Cyc_Absyn_Cast_e_struct*)
_temp288)->f2; goto _LL313;} else{ goto _LL314;} _LL314: if(*(( int*) _temp288)
== Cyc_Absyn_Address_e){ _LL392: _temp391=(( struct Cyc_Absyn_Address_e_struct*)
_temp288)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp288)
== Cyc_Absyn_Sizeofexp_e){ _LL394: _temp393=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp288)->f1; goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp288)
== Cyc_Absyn_Deref_e){ _LL396: _temp395=(( struct Cyc_Absyn_Deref_e_struct*)
_temp288)->f1; goto _LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp288)
== Cyc_Absyn_StructMember_e){ _LL400: _temp399=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp288)->f1; goto _LL398; _LL398: _temp397=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp288)->f2; goto _LL321;} else{ goto _LL322;} _LL322: if(*(( int*) _temp288)
== Cyc_Absyn_StructArrow_e){ _LL404: _temp403=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp288)->f1; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp288)->f2; goto _LL323;} else{ goto _LL324;} _LL324: if(*(( int*) _temp288)
== Cyc_Absyn_Increment_e){ _LL408: _temp407=(( struct Cyc_Absyn_Increment_e_struct*)
_temp288)->f1; goto _LL406; _LL406: _temp405=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp288)->f2; goto _LL325;} else{ goto _LL326;} _LL326: if(*(( int*) _temp288)
== Cyc_Absyn_Sizeoftyp_e){ _LL410: _temp409=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp288)->f1; goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp288)
== Cyc_Absyn_Offsetof_e){ _LL414: _temp413=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp288)->f1; goto _LL412; _LL412: _temp411=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp288)->f2; goto _LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp288)
== Cyc_Absyn_Enum_e){ _LL420: _temp419=(( struct Cyc_Absyn_Enum_e_struct*)
_temp288)->f1; goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_Enum_e_struct*)
_temp288)->f2; goto _LL416; _LL416: _temp415=(( struct Cyc_Absyn_Enum_e_struct*)
_temp288)->f3; goto _LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp288)
== Cyc_Absyn_UnresolvedMem_e){ _LL424: _temp423=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp288)->f1; goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp288)->f2; goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL291:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
varmap, _temp341)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( varmap,
_temp341))->r));} return; _LL293: return; _LL295: return; _LL297: Cyc_Tovc_apply_varmap(
varmap, _temp355); _temp357= _temp353; goto _LL299; _LL299: for( 0; _temp357 !=
0; _temp357=(( struct Cyc_List_List*) _check_null( _temp357))->tl){ Cyc_Tovc_apply_varmap(
varmap,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp357))->hd);}
return; _LL301: Cyc_Tovc_apply_varmap( varmap, _temp361); _temp369= _temp365;
_temp367= _temp363; goto _LL303; _LL303: _temp373= _temp369; _temp371= _temp367;
goto _LL305; _LL305: _temp379= _temp373; _temp375= _temp371; goto _LL307; _LL307:
Cyc_Tovc_apply_varmap( varmap, _temp375); _temp381= _temp379; goto _LL309;
_LL309: _temp385= _temp381; goto _LL311; _LL311: _temp387= _temp385; goto _LL313;
_LL313: _temp391= _temp387; goto _LL315; _LL315: _temp393= _temp391; goto _LL317;
_LL317: _temp395= _temp393; goto _LL319; _LL319: _temp399= _temp395; goto _LL321;
_LL321: _temp403= _temp399; goto _LL323; _LL323: _temp407= _temp403; goto _LL325;
_LL325: Cyc_Tovc_apply_varmap( varmap, _temp407); return; _LL327: return; _LL329:
return; _LL331: return; _LL333: for( 0; _temp421 != 0; _temp421=(( struct Cyc_List_List*)
_check_null( _temp421))->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp421))->hd)).f2);} return; _LL335:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp425=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp425[ 0]=({ struct
Cyc_Core_Impossible_struct _temp426; _temp426.tag= Cyc_Core_Impossible; _temp426.f1=
_tag_arr("bad exp form in Tocv::apply_varmap", sizeof( unsigned char), 35u);
_temp426;}); _temp425;})); _LL289:;} static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_exp(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){ void* _temp427=( void*)
s->r; struct Cyc_Absyn_Exp* _temp441; struct Cyc_Absyn_Stmt* _temp443; struct
Cyc_Absyn_Stmt* _temp445; struct Cyc_Absyn_Stmt* _temp447; struct Cyc_Absyn_Stmt*
_temp449; struct Cyc_Absyn_Exp* _temp451; struct Cyc_Absyn_Stmt* _temp453;
struct Cyc_Absyn_Decl* _temp455; _LL429: if( _temp427 ==( void*) Cyc_Absyn_Skip_s){
goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int) _temp427 > 1u?*((
int*) _temp427) == Cyc_Absyn_Exp_s: 0){ _LL442: _temp441=(( struct Cyc_Absyn_Exp_s_struct*)
_temp427)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp427 > 1u?*(( int*) _temp427) == Cyc_Absyn_Seq_s: 0){ _LL446: _temp445=((
struct Cyc_Absyn_Seq_s_struct*) _temp427)->f1; goto _LL444; _LL444: _temp443=((
struct Cyc_Absyn_Seq_s_struct*) _temp427)->f2; goto _LL434;} else{ goto _LL435;}
_LL435: if(( unsigned int) _temp427 > 1u?*(( int*) _temp427) == Cyc_Absyn_IfThenElse_s:
0){ _LL452: _temp451=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp427)->f1;
goto _LL450; _LL450: _temp449=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp427)->f2;
goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp427)->f3;
goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp427 > 1u?*((
int*) _temp427) == Cyc_Absyn_Decl_s: 0){ _LL456: _temp455=(( struct Cyc_Absyn_Decl_s_struct*)
_temp427)->f1; goto _LL454; _LL454: _temp453=(( struct Cyc_Absyn_Decl_s_struct*)
_temp427)->f2; goto _LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL430:
return Cyc_Absyn_true_exp( 0); _LL432: Cyc_Tovc_apply_varmap( env.varmap,
_temp441); return _temp441; _LL434: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp445), Cyc_Tovc_stmt_to_exp( env, _temp443), 0); _LL436: Cyc_Tovc_apply_varmap(
env.varmap, _temp451); return Cyc_Absyn_conditional_exp( _temp451, Cyc_Tovc_stmt_to_exp(
env, _temp449), Cyc_Tovc_stmt_to_exp( env, _temp447), 0); _LL438: { void*
_temp457=( void*) _temp455->r; struct Cyc_Absyn_Vardecl* _temp463; _LL459: if(*((
int*) _temp457) == Cyc_Absyn_Var_d){ _LL464: _temp463=(( struct Cyc_Absyn_Var_d_struct*)
_temp457)->f1; goto _LL460;} else{ goto _LL461;} _LL461: goto _LL462; _LL460: {
struct _tagged_arr* _temp465=(* _temp463->name).f2; struct _tuple0* _temp466=
Cyc_Toc_temp_var(); _temp463->name= _temp466; env.varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Exp* data))
Cyc_Dict_insert)( env.varmap, _temp465, Cyc_Absyn_var_exp( _temp466, 0)); if(
_temp463->initializer != 0){ Cyc_Tovc_apply_varmap( env.varmap,( struct Cyc_Absyn_Exp*)
_check_null( _temp463->initializer));{ void* _temp467=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp463->initializer))->r; struct Cyc_List_List* _temp473; struct
Cyc_Core_Opt* _temp475; _LL469: if(*(( int*) _temp467) == Cyc_Absyn_UnresolvedMem_e){
_LL476: _temp475=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp467)->f1;
goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp467)->f2; goto _LL470;} else{ goto _LL471;} _LL471: goto _LL472; _LL470: {
struct _tuple0* tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp463->type, _temp463->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp463->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp463->type);
goto _LL468;} _LL472: goto _LL468; _LL468:;}} env.all_locals=({ struct Cyc_List_List*
_temp477=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp477->hd=( void*) _temp463; _temp477->tl= env.all_locals; _temp477;});{
struct Cyc_Absyn_Exp* _temp478= _temp463->initializer; _temp463->initializer= 0;(
void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp455, Cyc_Absyn_new_stmt((
void*)( env.encloser)->r, 0), 0))->r));{ struct Cyc_Absyn_Exp* _temp479= Cyc_Tovc_stmt_to_exp(
env, _temp453); if( _temp478 != 0){ _temp479= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp463->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp478), 0), _temp479, 0);} return _temp479;}}} _LL462:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp480=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp480[ 0]=({ struct
Cyc_Core_Impossible_struct _temp481; _temp481.tag= Cyc_Core_Impossible; _temp481.f1=
_tag_arr("bad local decl in Tovc::stmt_to_exp", sizeof( unsigned char), 36u);
_temp481;}); _temp480;})); _LL458:;} _LL440: return Cyc_Tovc_stmt_to_fun( env, s,(
void*) Cyc_Absyn_VoidType); _LL428:;} static void Cyc_Tovc_stmt_to_vc( struct
Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s); static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Exp* e){ void* _temp482=( void*)
e->r; void* _temp530; void* _temp532; struct _tuple0* _temp534; struct Cyc_Absyn_VarargCallInfo*
_temp536; struct Cyc_List_List* _temp538; struct Cyc_Absyn_Exp* _temp540; struct
Cyc_List_List* _temp542; void* _temp544; struct Cyc_Absyn_Exp* _temp546; struct
Cyc_Absyn_Exp* _temp548; struct Cyc_Absyn_Exp* _temp550; struct Cyc_Absyn_Exp*
_temp552; struct Cyc_Absyn_Exp* _temp554; struct Cyc_Absyn_Exp* _temp556; struct
Cyc_Absyn_Exp* _temp558; struct Cyc_Absyn_Exp* _temp560; struct Cyc_Core_Opt*
_temp562; struct Cyc_Absyn_Exp* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_List_List* _temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp*
_temp572; void* _temp574; struct Cyc_Absyn_Exp* _temp576; struct Cyc_Absyn_Exp*
_temp578; struct Cyc_Absyn_Exp* _temp580; struct _tagged_arr* _temp582; struct
Cyc_Absyn_Exp* _temp584; struct _tagged_arr* _temp586; struct Cyc_Absyn_Exp*
_temp588; void* _temp590; struct Cyc_Absyn_Exp* _temp592; void* _temp594; struct
_tagged_arr* _temp596; void* _temp598; struct Cyc_Absyn_Enumfield* _temp600;
struct Cyc_Absyn_Enumdecl* _temp602; struct _tuple0* _temp604; struct Cyc_List_List*
_temp606; struct Cyc_Core_Opt* _temp608; struct Cyc_Absyn_Stmt* _temp610; _LL484:
if(*(( int*) _temp482) == Cyc_Absyn_Const_e){ _LL531: _temp530=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp482)->f1; goto _LL485;} else{ goto _LL486;}
_LL486: if(*(( int*) _temp482) == Cyc_Absyn_Var_e){ _LL535: _temp534=(( struct
Cyc_Absyn_Var_e_struct*) _temp482)->f1; goto _LL533; _LL533: _temp532=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp482)->f2; goto _LL487;} else{ goto _LL488;}
_LL488: if(*(( int*) _temp482) == Cyc_Absyn_FnCall_e){ _LL541: _temp540=((
struct Cyc_Absyn_FnCall_e_struct*) _temp482)->f1; goto _LL539; _LL539: _temp538=((
struct Cyc_Absyn_FnCall_e_struct*) _temp482)->f2; goto _LL537; _LL537: _temp536=((
struct Cyc_Absyn_FnCall_e_struct*) _temp482)->f3; goto _LL489;} else{ goto
_LL490;} _LL490: if(*(( int*) _temp482) == Cyc_Absyn_Primop_e){ _LL545: _temp544=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp482)->f1; goto _LL543; _LL543:
_temp542=(( struct Cyc_Absyn_Primop_e_struct*) _temp482)->f2; goto _LL491;}
else{ goto _LL492;} _LL492: if(*(( int*) _temp482) == Cyc_Absyn_Conditional_e){
_LL551: _temp550=(( struct Cyc_Absyn_Conditional_e_struct*) _temp482)->f1; goto
_LL549; _LL549: _temp548=(( struct Cyc_Absyn_Conditional_e_struct*) _temp482)->f2;
goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp482)->f3; goto _LL493;} else{ goto _LL494;} _LL494: if(*(( int*) _temp482)
== Cyc_Absyn_Subscript_e){ _LL555: _temp554=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp482)->f1; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp482)->f2; goto _LL495;} else{ goto _LL496;} _LL496: if(*(( int*) _temp482)
== Cyc_Absyn_SeqExp_e){ _LL559: _temp558=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp482)->f1; goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp482)->f2; goto _LL497;} else{ goto _LL498;} _LL498: if(*(( int*) _temp482)
== Cyc_Absyn_AssignOp_e){ _LL565: _temp564=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp482)->f1; goto _LL563; _LL563: _temp562=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp482)->f2; goto _LL561; _LL561: _temp560=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp482)->f3; goto _LL499;} else{ goto _LL500;} _LL500: if(*(( int*) _temp482)
== Cyc_Absyn_NoInstantiate_e){ _LL567: _temp566=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp482)->f1; goto _LL501;} else{ goto _LL502;} _LL502: if(*(( int*) _temp482)
== Cyc_Absyn_Instantiate_e){ _LL571: _temp570=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp482)->f1; goto _LL569; _LL569: _temp568=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp482)->f2; goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp482)
== Cyc_Absyn_Cast_e){ _LL575: _temp574=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp482)->f1; goto _LL573; _LL573: _temp572=(( struct Cyc_Absyn_Cast_e_struct*)
_temp482)->f2; goto _LL505;} else{ goto _LL506;} _LL506: if(*(( int*) _temp482)
== Cyc_Absyn_Address_e){ _LL577: _temp576=(( struct Cyc_Absyn_Address_e_struct*)
_temp482)->f1; goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp482)
== Cyc_Absyn_Sizeofexp_e){ _LL579: _temp578=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp482)->f1; goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp482)
== Cyc_Absyn_Deref_e){ _LL581: _temp580=(( struct Cyc_Absyn_Deref_e_struct*)
_temp482)->f1; goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp482)
== Cyc_Absyn_StructMember_e){ _LL585: _temp584=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp482)->f1; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp482)->f2; goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp482)
== Cyc_Absyn_StructArrow_e){ _LL589: _temp588=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp482)->f1; goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp482)->f2; goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp482)
== Cyc_Absyn_Increment_e){ _LL593: _temp592=(( struct Cyc_Absyn_Increment_e_struct*)
_temp482)->f1; goto _LL591; _LL591: _temp590=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp482)->f2; goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp482)
== Cyc_Absyn_Sizeoftyp_e){ _LL595: _temp594=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp482)->f1; goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp482)
== Cyc_Absyn_Offsetof_e){ _LL599: _temp598=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp482)->f1; goto _LL597; _LL597: _temp596=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp482)->f2; goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp482)
== Cyc_Absyn_Enum_e){ _LL605: _temp604=(( struct Cyc_Absyn_Enum_e_struct*)
_temp482)->f1; goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_Enum_e_struct*)
_temp482)->f2; goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_Enum_e_struct*)
_temp482)->f3; goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp482)
== Cyc_Absyn_UnresolvedMem_e){ _LL609: _temp608=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp482)->f1; goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp482)->f2; goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp482)
== Cyc_Absyn_StmtExp_e){ _LL611: _temp610=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp482)->f1; goto _LL527;} else{ goto _LL528;} _LL528: goto _LL529; _LL485:
return; _LL487: return; _LL489: Cyc_Tovc_exp_to_vc( env, _temp540); _temp542=
_temp538; goto _LL491; _LL491: for( 0; _temp542 != 0; _temp542=(( struct Cyc_List_List*)
_check_null( _temp542))->tl){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp542))->hd);} return; _LL493: Cyc_Tovc_exp_to_vc(
env, _temp546); _temp554= _temp550; _temp552= _temp548; goto _LL495; _LL495:
_temp558= _temp554; _temp556= _temp552; goto _LL497; _LL497: _temp564= _temp558;
_temp560= _temp556; goto _LL499; _LL499: Cyc_Tovc_exp_to_vc( env, _temp560);
_temp566= _temp564; goto _LL501; _LL501: _temp570= _temp566; goto _LL503; _LL503:
_temp572= _temp570; goto _LL505; _LL505: _temp576= _temp572; goto _LL507; _LL507:
_temp578= _temp576; goto _LL509; _LL509: _temp580= _temp578; goto _LL511; _LL511:
_temp584= _temp580; goto _LL513; _LL513: _temp588= _temp584; goto _LL515; _LL515:
_temp592= _temp588; goto _LL517; _LL517: Cyc_Tovc_exp_to_vc( env, _temp592);
return; _LL519: return; _LL521: return; _LL523: return; _LL525: for( 0; _temp606
!= 0; _temp606=(( struct Cyc_List_List*) _check_null( _temp606))->tl){ Cyc_Tovc_exp_to_vc(
env,(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp606))->hd)).f2);}
return; _LL527: Cyc_Tovc_stmt_to_vc( env, _temp610);( void*)( e->r=( void*)((
void*)( Cyc_Tovc_stmt_to_exp( env, _temp610))->r)); return; _LL529:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp612=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp612[ 0]=({ struct
Cyc_Core_Impossible_struct _temp613; _temp613.tag= Cyc_Core_Impossible; _temp613.f1=
_tag_arr("bad exp form after xlation to C", sizeof( unsigned char), 32u);
_temp613;}); _temp612;})); _LL483:;} static void Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s){ while( 1) { env.encloser= s;{ void* _temp614=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp646; struct Cyc_Absyn_Stmt* _temp648;
struct Cyc_Absyn_Stmt* _temp650; struct _tagged_arr* _temp652; struct Cyc_Absyn_Stmt*
_temp654; struct _tagged_arr* _temp656; struct Cyc_Absyn_Exp* _temp658; struct
Cyc_Absyn_Exp* _temp660; struct Cyc_Absyn_Stmt* _temp662; struct Cyc_Absyn_Stmt*
_temp664; struct Cyc_Absyn_Stmt* _temp666; struct Cyc_Absyn_Stmt* _temp668;
struct Cyc_Absyn_Exp* _temp670; struct Cyc_Absyn_Stmt* _temp672; struct _tuple2
_temp674; struct Cyc_Absyn_Stmt* _temp676; struct Cyc_Absyn_Exp* _temp678;
struct Cyc_Absyn_Stmt* _temp680; struct _tuple2 _temp682; struct Cyc_Absyn_Stmt*
_temp684; struct Cyc_Absyn_Exp* _temp686; struct _tuple2 _temp688; struct Cyc_Absyn_Stmt*
_temp690; struct Cyc_Absyn_Exp* _temp692; struct Cyc_Absyn_Exp* _temp694; struct
_tuple2 _temp696; struct Cyc_Absyn_Stmt* _temp698; struct Cyc_Absyn_Exp*
_temp700; struct Cyc_Absyn_Stmt* _temp702; struct Cyc_List_List* _temp704;
struct Cyc_Absyn_Exp* _temp706; struct Cyc_Absyn_Stmt* _temp708; struct Cyc_Absyn_Decl*
_temp710; _LL616: if( _temp614 ==( void*) Cyc_Absyn_Skip_s){ goto _LL617;} else{
goto _LL618;} _LL618: if(( unsigned int) _temp614 > 1u?*(( int*) _temp614) ==
Cyc_Absyn_Break_s: 0){ _LL647: _temp646=(( struct Cyc_Absyn_Break_s_struct*)
_temp614)->f1; goto _LL619;} else{ goto _LL620;} _LL620: if(( unsigned int)
_temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Continue_s: 0){ _LL649: _temp648=((
struct Cyc_Absyn_Continue_s_struct*) _temp614)->f1; goto _LL621;} else{ goto
_LL622;} _LL622: if(( unsigned int) _temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Goto_s:
0){ _LL653: _temp652=(( struct Cyc_Absyn_Goto_s_struct*) _temp614)->f1; goto
_LL651; _LL651: _temp650=(( struct Cyc_Absyn_Goto_s_struct*) _temp614)->f2; goto
_LL623;} else{ goto _LL624;} _LL624: if(( unsigned int) _temp614 > 1u?*(( int*)
_temp614) == Cyc_Absyn_Label_s: 0){ _LL657: _temp656=(( struct Cyc_Absyn_Label_s_struct*)
_temp614)->f1; goto _LL655; _LL655: _temp654=(( struct Cyc_Absyn_Label_s_struct*)
_temp614)->f2; goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int)
_temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Return_s: 0){ _LL659: _temp658=((
struct Cyc_Absyn_Return_s_struct*) _temp614)->f1; goto _LL627;} else{ goto
_LL628;} _LL628: if(( unsigned int) _temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Exp_s:
0){ _LL661: _temp660=(( struct Cyc_Absyn_Exp_s_struct*) _temp614)->f1; goto
_LL629;} else{ goto _LL630;} _LL630: if(( unsigned int) _temp614 > 1u?*(( int*)
_temp614) == Cyc_Absyn_Seq_s: 0){ _LL665: _temp664=(( struct Cyc_Absyn_Seq_s_struct*)
_temp614)->f1; goto _LL663; _LL663: _temp662=(( struct Cyc_Absyn_Seq_s_struct*)
_temp614)->f2; goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int)
_temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_IfThenElse_s: 0){ _LL671:
_temp670=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp614)->f1; goto _LL669;
_LL669: _temp668=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp614)->f2; goto
_LL667; _LL667: _temp666=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp614)->f3;
goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int) _temp614 > 1u?*((
int*) _temp614) == Cyc_Absyn_While_s: 0){ _LL675: _temp674=(( struct Cyc_Absyn_While_s_struct*)
_temp614)->f1; _LL679: _temp678= _temp674.f1; goto _LL677; _LL677: _temp676=
_temp674.f2; goto _LL673; _LL673: _temp672=(( struct Cyc_Absyn_While_s_struct*)
_temp614)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_For_s: 0){ _LL695: _temp694=((
struct Cyc_Absyn_For_s_struct*) _temp614)->f1; goto _LL689; _LL689: _temp688=((
struct Cyc_Absyn_For_s_struct*) _temp614)->f2; _LL693: _temp692= _temp688.f1;
goto _LL691; _LL691: _temp690= _temp688.f2; goto _LL683; _LL683: _temp682=((
struct Cyc_Absyn_For_s_struct*) _temp614)->f3; _LL687: _temp686= _temp682.f1;
goto _LL685; _LL685: _temp684= _temp682.f2; goto _LL681; _LL681: _temp680=((
struct Cyc_Absyn_For_s_struct*) _temp614)->f4; goto _LL637;} else{ goto _LL638;}
_LL638: if(( unsigned int) _temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Do_s:
0){ _LL703: _temp702=(( struct Cyc_Absyn_Do_s_struct*) _temp614)->f1; goto
_LL697; _LL697: _temp696=(( struct Cyc_Absyn_Do_s_struct*) _temp614)->f2; _LL701:
_temp700= _temp696.f1; goto _LL699; _LL699: _temp698= _temp696.f2; goto _LL639;}
else{ goto _LL640;} _LL640: if(( unsigned int) _temp614 > 1u?*(( int*) _temp614)
== Cyc_Absyn_Switch_s: 0){ _LL707: _temp706=(( struct Cyc_Absyn_Switch_s_struct*)
_temp614)->f1; goto _LL705; _LL705: _temp704=(( struct Cyc_Absyn_Switch_s_struct*)
_temp614)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Decl_s: 0){ _LL711: _temp710=((
struct Cyc_Absyn_Decl_s_struct*) _temp614)->f1; goto _LL709; _LL709: _temp708=((
struct Cyc_Absyn_Decl_s_struct*) _temp614)->f2; goto _LL643;} else{ goto _LL644;}
_LL644: goto _LL645; _LL617: return; _LL619: return; _LL621: return; _LL623:
return; _LL625: s= _temp654; continue; _LL627: if( _temp658 == 0){ return;}
_temp660=( struct Cyc_Absyn_Exp*) _check_null( _temp658); goto _LL629; _LL629:
Cyc_Tovc_exp_to_vc( env, _temp660); return; _LL631: Cyc_Tovc_stmt_to_vc( env,
_temp664); s= _temp662; continue; _LL633: Cyc_Tovc_exp_to_vc( env, _temp670);
Cyc_Tovc_stmt_to_vc( env, _temp668); s= _temp666; continue; _LL635: Cyc_Tovc_exp_to_vc(
env, _temp678); s= _temp672; continue; _LL637: Cyc_Tovc_exp_to_vc( env, _temp694);
Cyc_Tovc_exp_to_vc( env, _temp692); Cyc_Tovc_exp_to_vc( env, _temp686); s=
_temp680; continue; _LL639: Cyc_Tovc_exp_to_vc( env, _temp700); s= _temp702;
continue; _LL641: Cyc_Tovc_exp_to_vc( env, _temp706); for( 0; _temp704 != 0;
_temp704=(( struct Cyc_List_List*) _check_null( _temp704))->tl){ Cyc_Tovc_stmt_to_vc(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp704))->hd)->body);} return; _LL643:{ void* _temp712=( void*) _temp710->r;
struct Cyc_Absyn_Vardecl* _temp718; _LL714: if(*(( int*) _temp712) == Cyc_Absyn_Var_d){
_LL719: _temp718=(( struct Cyc_Absyn_Var_d_struct*) _temp712)->f1; goto _LL715;}
else{ goto _LL716;} _LL716: goto _LL717; _LL715:{ void* _temp720= Cyc_Tcutil_compress((
void*) _temp718->type); struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Tqual
_temp728; void* _temp730; _LL722: if(( unsigned int) _temp720 > 4u?*(( int*)
_temp720) == Cyc_Absyn_ArrayType: 0){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp720)->f1; goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_ArrayType_struct*)
_temp720)->f2; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_ArrayType_struct*)
_temp720)->f3; goto _LL723;} else{ goto _LL724;} _LL724: goto _LL725; _LL723:
if( _temp718->initializer != 0){ void* _temp732=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp718->initializer))->r; struct Cyc_List_List* _temp738; struct
Cyc_Core_Opt* _temp740; _LL734: if(*(( int*) _temp732) == Cyc_Absyn_UnresolvedMem_e){
_LL741: _temp740=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp732)->f1;
goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp732)->f2; goto _LL735;} else{ goto _LL736;} _LL736: goto _LL737; _LL735:
_temp718->initializer= 0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp(
_temp718->name, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_new_stmt(( void*)
_temp708->r, 0);{ unsigned int i= 0; for( 0; _temp738 != 0;( _temp738=(( struct
Cyc_List_List*) _check_null( _temp738))->tl, ++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp( arr, Cyc_Absyn_uint_exp( i, 0), 0),(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp738))->hd)).f2, 0),
0), s3, 0);}}( void*)( _temp708->r=( void*)(( void*) s3->r)); goto _LL733;}
_LL737:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp742=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp742[ 0]=({ struct Cyc_Core_Impossible_struct _temp743; _temp743.tag= Cyc_Core_Impossible;
_temp743.f1= _tag_arr("array type bad initializer", sizeof( unsigned char), 27u);
_temp743;}); _temp742;})); _LL733:;} goto _LL721; _LL725: goto _LL721; _LL721:;}
env.all_locals=({ struct Cyc_List_List* _temp744=( struct Cyc_List_List*)
_region_malloc( env.rgn, sizeof( struct Cyc_List_List)); _temp744->hd=( void*)
_temp718; _temp744->tl= env.all_locals; _temp744;}); if( _temp718->initializer
!= 0){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*) _check_null( _temp718->initializer));}
s= _temp708; continue; _LL717: goto _LL713; _LL713:;} goto _LL645; _LL645:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp745=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp745[ 0]=({ struct
Cyc_Core_Impossible_struct _temp746; _temp746.tag= Cyc_Core_Impossible; _temp746.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp746;});
_temp745;})); _LL615:;}}} struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List*
old_ds){ struct Cyc_List_List* _temp747= 0; for( 0; old_ds != 0; old_ds=((
struct Cyc_List_List*) _check_null( old_ds))->tl){ struct Cyc_Absyn_Decl*
_temp748=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( old_ds))->hd;
void* _temp749=( void*) _temp748->r; struct Cyc_Absyn_Fndecl* _temp775; struct
Cyc_Absyn_Vardecl* _temp777; int _temp779; struct Cyc_Absyn_Exp* _temp781;
struct Cyc_Core_Opt* _temp783; struct Cyc_Core_Opt* _temp785; struct Cyc_Absyn_Pat*
_temp787; struct Cyc_List_List* _temp789; struct Cyc_Absyn_Structdecl* _temp791;
struct Cyc_Absyn_Uniondecl* _temp793; struct Cyc_Absyn_Tuniondecl* _temp795;
struct Cyc_Absyn_Enumdecl* _temp797; struct Cyc_Absyn_Typedefdecl* _temp799;
struct Cyc_List_List* _temp801; struct _tagged_arr* _temp803; struct Cyc_List_List*
_temp805; struct _tuple0* _temp807; struct Cyc_List_List* _temp809; _LL751: if(*((
int*) _temp749) == Cyc_Absyn_Fn_d){ _LL776: _temp775=(( struct Cyc_Absyn_Fn_d_struct*)
_temp749)->f1; goto _LL752;} else{ goto _LL753;} _LL753: if(*(( int*) _temp749)
== Cyc_Absyn_Var_d){ _LL778: _temp777=(( struct Cyc_Absyn_Var_d_struct*)
_temp749)->f1; goto _LL754;} else{ goto _LL755;} _LL755: if(*(( int*) _temp749)
== Cyc_Absyn_Let_d){ _LL788: _temp787=(( struct Cyc_Absyn_Let_d_struct*)
_temp749)->f1; goto _LL786; _LL786: _temp785=(( struct Cyc_Absyn_Let_d_struct*)
_temp749)->f2; goto _LL784; _LL784: _temp783=(( struct Cyc_Absyn_Let_d_struct*)
_temp749)->f3; goto _LL782; _LL782: _temp781=(( struct Cyc_Absyn_Let_d_struct*)
_temp749)->f4; goto _LL780; _LL780: _temp779=(( struct Cyc_Absyn_Let_d_struct*)
_temp749)->f5; goto _LL756;} else{ goto _LL757;} _LL757: if(*(( int*) _temp749)
== Cyc_Absyn_Letv_d){ _LL790: _temp789=(( struct Cyc_Absyn_Letv_d_struct*)
_temp749)->f1; goto _LL758;} else{ goto _LL759;} _LL759: if(*(( int*) _temp749)
== Cyc_Absyn_Struct_d){ _LL792: _temp791=(( struct Cyc_Absyn_Struct_d_struct*)
_temp749)->f1; goto _LL760;} else{ goto _LL761;} _LL761: if(*(( int*) _temp749)
== Cyc_Absyn_Union_d){ _LL794: _temp793=(( struct Cyc_Absyn_Union_d_struct*)
_temp749)->f1; goto _LL762;} else{ goto _LL763;} _LL763: if(*(( int*) _temp749)
== Cyc_Absyn_Tunion_d){ _LL796: _temp795=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp749)->f1; goto _LL764;} else{ goto _LL765;} _LL765: if(*(( int*) _temp749)
== Cyc_Absyn_Enum_d){ _LL798: _temp797=(( struct Cyc_Absyn_Enum_d_struct*)
_temp749)->f1; goto _LL766;} else{ goto _LL767;} _LL767: if(*(( int*) _temp749)
== Cyc_Absyn_Typedef_d){ _LL800: _temp799=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp749)->f1; goto _LL768;} else{ goto _LL769;} _LL769: if(*(( int*) _temp749)
== Cyc_Absyn_Namespace_d){ _LL804: _temp803=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp749)->f1; goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp749)->f2; goto _LL770;} else{ goto _LL771;} _LL771: if(*(( int*) _temp749)
== Cyc_Absyn_Using_d){ _LL808: _temp807=(( struct Cyc_Absyn_Using_d_struct*)
_temp749)->f1; goto _LL806; _LL806: _temp805=(( struct Cyc_Absyn_Using_d_struct*)
_temp749)->f2; goto _LL772;} else{ goto _LL773;} _LL773: if(*(( int*) _temp749)
== Cyc_Absyn_ExternC_d){ _LL810: _temp809=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp749)->f1; goto _LL774;} else{ goto _LL750;} _LL752:{ struct _RegionHandle
_temp811= _new_region(); struct _RegionHandle* rgn=& _temp811; _push_region( rgn);{
struct Cyc_List_List** _temp812=({ struct Cyc_List_List** _temp814=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp814[ 0]= 0; _temp814;});
Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp813; _temp813.rgn= rgn;
_temp813.all_locals=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
Cyc_List_List* src)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp775->param_vardecls))->v); _temp813.varmap=(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*, int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_rempty)( rgn, Cyc_String_zstrptrcmp); _temp813.encloser= _temp775->body;
_temp813.gen_ds= _temp812; _temp813;}), _temp775->body); _temp747=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(* _temp812,
_temp747);}; _pop_region( rgn);} goto _LL754; _LL754: goto _LL756; _LL756: goto
_LL758; _LL758: goto _LL760; _LL760: goto _LL762; _LL762: goto _LL764; _LL764:
goto _LL766; _LL766: goto _LL768; _LL768: _temp747=({ struct Cyc_List_List*
_temp815=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp815->hd=( void*) _temp748; _temp815->tl= _temp747; _temp815;}); goto _LL750;
_LL770: _temp805= _temp801; goto _LL772; _LL772: _temp809= _temp805; goto _LL774;
_LL774:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp816=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp816[ 0]=({ struct Cyc_Core_Impossible_struct _temp817; _temp817.tag= Cyc_Core_Impossible;
_temp817.f1= _tag_arr("nested translation unit after translation to C", sizeof(
unsigned char), 47u); _temp817;}); _temp816;})); _LL750:;} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp747);}