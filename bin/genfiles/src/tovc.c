 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern int Cyc_Core_ptrcmp( void**, void**); extern unsigned char
Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag;
struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string f1; }
; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_rcopy( struct _RegionHandle*, struct Cyc_List_List*
src); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_rempty(
struct _RegionHandle*, int(* comp)( void*, void*)); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict*, void*); struct Cyc_Stdio___sFILE; extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
_temp48; struct _tuple0* _temp50; struct _tuple0 _temp52; struct _tagged_string*
_temp53; void* _temp55; void* _temp57; struct _tuple0* _temp59; void* _temp61;
struct Cyc_Absyn_VarargCallInfo* _temp63; struct Cyc_List_List* _temp65; struct
Cyc_Absyn_Exp* _temp67; struct Cyc_List_List* _temp69; void* _temp71; struct Cyc_Absyn_Exp*
_temp73; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp* _temp77; struct
Cyc_Absyn_Exp* _temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
Cyc_Core_Opt* _temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Exp*
_temp93; struct Cyc_List_List* _temp95; struct Cyc_Absyn_Exp* _temp97; struct
Cyc_Absyn_Exp* _temp99; void* _temp101; struct Cyc_Absyn_Exp* _temp103; struct
Cyc_Absyn_Exp* _temp105; struct Cyc_Absyn_Exp* _temp107; struct _tagged_string*
_temp109; struct Cyc_Absyn_Exp* _temp111; struct _tagged_string* _temp113;
struct Cyc_Absyn_Exp* _temp115; void* _temp117; struct Cyc_Absyn_Exp* _temp119;
void* _temp121; struct _tagged_string* _temp123; void* _temp125; struct Cyc_Absyn_Enumfield*
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
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( env.varmap,
_temp53)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( env.varmap,
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
Cyc_Set_rinsert)( env.rgn,* env.freevars, _temp137); return;}}( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp151=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp151[ 0]=({ struct
Cyc_Core_Impossible_struct _temp152; _temp152.tag= Cyc_Core_Impossible; _temp152.f1=(
struct _tagged_string)({ struct _tagged_string _temp153=* _temp53; xprintf("unbound variable %.*s found in box_free_vars_exp",
_temp153.last_plus_one - _temp153.curr, _temp153.curr);}); _temp152;}); _temp151;}));
_LL5: return; _LL7: return; _LL9: Cyc_Tovc_box_free_vars_exp( env, _temp67);
_temp69= _temp65; goto _LL11; _LL11: for( 0; _temp69 != 0; _temp69=(( struct Cyc_List_List*)
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
return; _LL47:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp154=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp154[ 0]=({ struct Cyc_Core_Impossible_struct _temp155; _temp155.tag= Cyc_Core_Impossible;
_temp155.f1=({ unsigned char* _temp156=( unsigned char*)"bad exp form in Tocv::box_free_vars_exp";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 40; _temp157;}); _temp155;}); _temp154;}));
_LL1:;}} static void Cyc_Tovc_box_free_vars_stmt( struct Cyc_Tovc_BoxingEnv env,
struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp158=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp190; struct Cyc_Absyn_Stmt* _temp192; struct Cyc_Absyn_Stmt* _temp194;
struct _tagged_string* _temp196; struct Cyc_Absyn_Stmt* _temp198; struct
_tagged_string* _temp200; struct Cyc_Absyn_Exp* _temp202; struct Cyc_Absyn_Exp*
_temp204; struct Cyc_Absyn_Stmt* _temp206; struct Cyc_Absyn_Stmt* _temp208;
struct Cyc_Absyn_Stmt* _temp210; struct Cyc_Absyn_Stmt* _temp212; struct Cyc_Absyn_Exp*
_temp214; struct Cyc_Absyn_Stmt* _temp216; struct _tuple2 _temp218; struct Cyc_Absyn_Stmt*
_temp220; struct Cyc_Absyn_Exp* _temp222; struct Cyc_Absyn_Stmt* _temp224;
struct _tuple2 _temp226; struct Cyc_Absyn_Stmt* _temp228; struct Cyc_Absyn_Exp*
_temp230; struct _tuple2 _temp232; struct Cyc_Absyn_Stmt* _temp234; struct Cyc_Absyn_Exp*
_temp236; struct Cyc_Absyn_Exp* _temp238; struct _tuple2 _temp240; struct Cyc_Absyn_Stmt*
_temp242; struct Cyc_Absyn_Exp* _temp244; struct Cyc_Absyn_Stmt* _temp246;
struct Cyc_List_List* _temp248; struct Cyc_Absyn_Exp* _temp250; struct Cyc_Absyn_Stmt*
_temp252; struct Cyc_Absyn_Decl* _temp254; _LL160: if( _temp158 ==( void*) Cyc_Absyn_Skip_s){
goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp158 > 1u?*((
int*) _temp158) == Cyc_Absyn_Break_s: 0){ _LL191: _temp190=(( struct Cyc_Absyn_Break_s_struct*)
_temp158)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Continue_s: 0){ _LL193: _temp192=((
struct Cyc_Absyn_Continue_s_struct*) _temp158)->f1; goto _LL165;} else{ goto
_LL166;} _LL166: if(( unsigned int) _temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Goto_s:
0){ _LL197: _temp196=(( struct Cyc_Absyn_Goto_s_struct*) _temp158)->f1; goto
_LL195; _LL195: _temp194=(( struct Cyc_Absyn_Goto_s_struct*) _temp158)->f2; goto
_LL167;} else{ goto _LL168;} _LL168: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_Label_s: 0){ _LL201: _temp200=(( struct Cyc_Absyn_Label_s_struct*)
_temp158)->f1; goto _LL199; _LL199: _temp198=(( struct Cyc_Absyn_Label_s_struct*)
_temp158)->f2; goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Return_s: 0){ _LL203: _temp202=((
struct Cyc_Absyn_Return_s_struct*) _temp158)->f1; goto _LL171;} else{ goto
_LL172;} _LL172: if(( unsigned int) _temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Exp_s:
0){ _LL205: _temp204=(( struct Cyc_Absyn_Exp_s_struct*) _temp158)->f1; goto
_LL173;} else{ goto _LL174;} _LL174: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_Seq_s: 0){ _LL209: _temp208=(( struct Cyc_Absyn_Seq_s_struct*)
_temp158)->f1; goto _LL207; _LL207: _temp206=(( struct Cyc_Absyn_Seq_s_struct*)
_temp158)->f2; goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_IfThenElse_s: 0){ _LL215:
_temp214=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp158)->f1; goto _LL213;
_LL213: _temp212=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp158)->f2; goto
_LL211; _LL211: _temp210=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp158)->f3;
goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int) _temp158 > 1u?*((
int*) _temp158) == Cyc_Absyn_While_s: 0){ _LL219: _temp218=(( struct Cyc_Absyn_While_s_struct*)
_temp158)->f1; _LL223: _temp222= _temp218.f1; goto _LL221; _LL221: _temp220=
_temp218.f2; goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_While_s_struct*)
_temp158)->f2; goto _LL179;} else{ goto _LL180;} _LL180: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_For_s: 0){ _LL239: _temp238=((
struct Cyc_Absyn_For_s_struct*) _temp158)->f1; goto _LL233; _LL233: _temp232=((
struct Cyc_Absyn_For_s_struct*) _temp158)->f2; _LL237: _temp236= _temp232.f1;
goto _LL235; _LL235: _temp234= _temp232.f2; goto _LL227; _LL227: _temp226=((
struct Cyc_Absyn_For_s_struct*) _temp158)->f3; _LL231: _temp230= _temp226.f1;
goto _LL229; _LL229: _temp228= _temp226.f2; goto _LL225; _LL225: _temp224=((
struct Cyc_Absyn_For_s_struct*) _temp158)->f4; goto _LL181;} else{ goto _LL182;}
_LL182: if(( unsigned int) _temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Do_s:
0){ _LL247: _temp246=(( struct Cyc_Absyn_Do_s_struct*) _temp158)->f1; goto
_LL241; _LL241: _temp240=(( struct Cyc_Absyn_Do_s_struct*) _temp158)->f2; _LL245:
_temp244= _temp240.f1; goto _LL243; _LL243: _temp242= _temp240.f2; goto _LL183;}
else{ goto _LL184;} _LL184: if(( unsigned int) _temp158 > 1u?*(( int*) _temp158)
== Cyc_Absyn_Switch_s: 0){ _LL251: _temp250=(( struct Cyc_Absyn_Switch_s_struct*)
_temp158)->f1; goto _LL249; _LL249: _temp248=(( struct Cyc_Absyn_Switch_s_struct*)
_temp158)->f2; goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Decl_s: 0){ _LL255: _temp254=((
struct Cyc_Absyn_Decl_s_struct*) _temp158)->f1; goto _LL253; _LL253: _temp252=((
struct Cyc_Absyn_Decl_s_struct*) _temp158)->f2; goto _LL187;} else{ goto _LL188;}
_LL188: goto _LL189; _LL161: return; _LL163: return; _LL165: return; _LL167:
return; _LL169: s= _temp198; continue; _LL171: if( _temp202 == 0){ return;}
_temp204=( struct Cyc_Absyn_Exp*) _check_null( _temp202); goto _LL173; _LL173:
Cyc_Tovc_box_free_vars_exp( env, _temp204); return; _LL175: Cyc_Tovc_box_free_vars_stmt(
env, _temp208); s= _temp206; continue; _LL177: Cyc_Tovc_box_free_vars_exp( env,
_temp214); Cyc_Tovc_box_free_vars_stmt( env, _temp212); s= _temp210; continue;
_LL179: Cyc_Tovc_box_free_vars_exp( env, _temp222); s= _temp216; continue;
_LL181: Cyc_Tovc_box_free_vars_exp( env, _temp238); Cyc_Tovc_box_free_vars_exp(
env, _temp236); Cyc_Tovc_box_free_vars_exp( env, _temp230); s= _temp224;
continue; _LL183: Cyc_Tovc_box_free_vars_exp( env, _temp244); s= _temp246;
continue; _LL185: Cyc_Tovc_box_free_vars_exp( env, _temp250); for( 0; _temp248
!= 0; _temp248=(( struct Cyc_List_List*) _check_null( _temp248))->tl){ Cyc_Tovc_box_free_vars_stmt(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp248))->hd)->body);} return; _LL187:{ void* _temp256=( void*) _temp254->r;
struct Cyc_Absyn_Vardecl* _temp262; _LL258: if(*(( int*) _temp256) == Cyc_Absyn_Var_d){
_LL263: _temp262=(( struct Cyc_Absyn_Var_d_struct*) _temp256)->f1; goto _LL259;}
else{ goto _LL260;} _LL260: goto _LL261; _LL259: env.boundvars=({ struct Cyc_List_List*
_temp264=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp264->hd=( void*) _temp262; _temp264->tl= env.boundvars; _temp264;}); env.varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*, struct _tagged_string*)) Cyc_Dict_rdelete_same)(
env.varmap,(* _temp262->name).f2); if( _temp262->initializer != 0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp262->initializer));} s= _temp252;
continue; _LL261: goto _LL257; _LL257:;} goto _LL189; _LL189:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp265=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp265[ 0]=({ struct
Cyc_Core_Impossible_struct _temp266; _temp266.tag= Cyc_Core_Impossible; _temp266.f1=({
unsigned char* _temp267=( unsigned char*)"bad stmt after xlation to C"; struct
_tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 28; _temp268;}); _temp266;}); _temp265;}));
_LL159:;}} struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2;
void* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* rgn, int(* comp)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*))) Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp270=({
struct Cyc_Tovc_BoxingEnv _temp269; _temp269.rgn= env.rgn; _temp269.all_locals=
env.all_locals; _temp269.varmap= env.varmap; _temp269.boundvars= 0; _temp269.freevars=&
freevars; _temp269;}); goto _LL271; _LL271: Cyc_Tovc_box_free_vars_stmt(
_temp270, s);{ struct Cyc_List_List* params= 0; struct Cyc_List_List* args= 0;
struct Cyc_List_List* free_vars=(( struct Cyc_List_List*(*)( struct Cyc_Set_Set*
s)) Cyc_Set_elements)( freevars); for( 0; free_vars != 0; free_vars=(( struct
Cyc_List_List*) _check_null( free_vars))->tl){ struct Cyc_Absyn_Vardecl* vd=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( free_vars))->hd;
void* _temp272=( void*) vd->type; struct Cyc_Absyn_Exp* _temp278; struct Cyc_Absyn_Tqual
_temp280; void* _temp282; _LL274: if(( unsigned int) _temp272 > 4u?*(( int*)
_temp272) == Cyc_Absyn_ArrayType: 0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp272)->f1; goto _LL281; _LL281: _temp280=(( struct Cyc_Absyn_ArrayType_struct*)
_temp272)->f2; goto _LL279; _LL279: _temp278=(( struct Cyc_Absyn_ArrayType_struct*)
_temp272)->f3; goto _LL275;} else{ goto _LL276;} _LL276: goto _LL277; _LL275:
params=({ struct Cyc_List_List* _temp284=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp284->hd=( void*)({ struct _tuple4* _temp285=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp285->f1=(* vd->name).f2;
_temp285->f2= vd->tq; _temp285->f3= Cyc_Absyn_cstar_typ( _temp282, Cyc_Absyn_empty_tqual());
_temp285;}); _temp284->tl= params; _temp284;}); args=({ struct Cyc_List_List*
_temp286=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp286->hd=( void*) Cyc_Absyn_var_exp( vd->name, 0); _temp286->tl= args;
_temp286;}); goto _LL273; _LL277: params=({ struct Cyc_List_List* _temp287=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp287->hd=(
void*)({ struct _tuple4* _temp288=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp288->f1=(* vd->name).f2; _temp288->f2= vd->tq; _temp288->f3= Cyc_Absyn_cstar_typ((
void*) vd->type, Cyc_Absyn_empty_tqual()); _temp288;}); _temp287->tl= params;
_temp287;}); args=({ struct Cyc_List_List* _temp289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp289->hd=( void*) Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp( vd->name, 0), 0); _temp289->tl= args; _temp289;}); goto
_LL273; _LL273:;}{ struct _tuple0* funname= Cyc_Toc_temp_var();* env.gen_ds=({
struct Cyc_List_List* _temp290=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp290->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp291=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp291[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp292; _temp292.tag= Cyc_Absyn_Fn_d; _temp292.f1=({ struct Cyc_Absyn_Fndecl*
_temp293=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp293->sc=( void*)(( void*) Cyc_Absyn_Static); _temp293->is_inline= 1;
_temp293->name= funname; _temp293->tvs= 0; _temp293->effect= 0; _temp293->ret_type=(
void*) rettype; _temp293->args= params; _temp293->c_varargs= 0; _temp293->cyc_varargs=
0; _temp293->rgn_po= 0; _temp293->body= s; _temp293->cached_typ= 0; _temp293->param_vardecls=
0; _temp293->attributes= 0; _temp293;}); _temp292;}); _temp291;}), 0); _temp290->tl=*
env.gen_ds; _temp290;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp294=( void*) e->r; void* _temp342; struct
_tuple0* _temp344; struct _tuple0 _temp346; struct _tagged_string* _temp347;
void* _temp349; void* _temp351; struct _tuple0* _temp353; void* _temp355; struct
Cyc_Absyn_VarargCallInfo* _temp357; struct Cyc_List_List* _temp359; struct Cyc_Absyn_Exp*
_temp361; struct Cyc_List_List* _temp363; void* _temp365; struct Cyc_Absyn_Exp*
_temp367; struct Cyc_Absyn_Exp* _temp369; struct Cyc_Absyn_Exp* _temp371; struct
Cyc_Absyn_Exp* _temp373; struct Cyc_Absyn_Exp* _temp375; struct Cyc_Absyn_Exp*
_temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_Absyn_Exp* _temp381; struct
Cyc_Core_Opt* _temp383; struct Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Exp*
_temp387; struct Cyc_List_List* _temp389; struct Cyc_Absyn_Exp* _temp391; struct
Cyc_Absyn_Exp* _temp393; void* _temp395; struct Cyc_Absyn_Exp* _temp397; struct
Cyc_Absyn_Exp* _temp399; struct Cyc_Absyn_Exp* _temp401; struct _tagged_string*
_temp403; struct Cyc_Absyn_Exp* _temp405; struct _tagged_string* _temp407;
struct Cyc_Absyn_Exp* _temp409; void* _temp411; struct Cyc_Absyn_Exp* _temp413;
void* _temp415; struct _tagged_string* _temp417; void* _temp419; struct Cyc_Absyn_Enumfield*
_temp421; struct Cyc_Absyn_Enumdecl* _temp423; struct _tuple0* _temp425; struct
Cyc_List_List* _temp427; struct Cyc_Core_Opt* _temp429; _LL296: if(*(( int*)
_temp294) == Cyc_Absyn_Var_e){ _LL345: _temp344=(( struct Cyc_Absyn_Var_e_struct*)
_temp294)->f1; _temp346=* _temp344; _LL350: _temp349= _temp346.f1; if( _temp349
==( void*) Cyc_Absyn_Loc_n){ goto _LL348;} else{ goto _LL298;} _LL348: _temp347=
_temp346.f2; goto _LL343; _LL343: _temp342=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp294)->f2; goto _LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp294)
== Cyc_Absyn_Var_e){ _LL354: _temp353=(( struct Cyc_Absyn_Var_e_struct*)
_temp294)->f1; goto _LL352; _LL352: _temp351=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp294)->f2; goto _LL299;} else{ goto _LL300;} _LL300: if(*(( int*) _temp294)
== Cyc_Absyn_Const_e){ _LL356: _temp355=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp294)->f1; goto _LL301;} else{ goto _LL302;} _LL302: if(*(( int*) _temp294)
== Cyc_Absyn_FnCall_e){ _LL362: _temp361=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp294)->f1; goto _LL360; _LL360: _temp359=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp294)->f2; goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp294)->f3; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*) _temp294)
== Cyc_Absyn_Primop_e){ _LL366: _temp365=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp294)->f1; goto _LL364; _LL364: _temp363=(( struct Cyc_Absyn_Primop_e_struct*)
_temp294)->f2; goto _LL305;} else{ goto _LL306;} _LL306: if(*(( int*) _temp294)
== Cyc_Absyn_Conditional_e){ _LL372: _temp371=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp294)->f1; goto _LL370; _LL370: _temp369=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp294)->f2; goto _LL368; _LL368: _temp367=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp294)->f3; goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp294)
== Cyc_Absyn_Subscript_e){ _LL376: _temp375=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp294)->f1; goto _LL374; _LL374: _temp373=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp294)->f2; goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp294)
== Cyc_Absyn_SeqExp_e){ _LL380: _temp379=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp294)->f1; goto _LL378; _LL378: _temp377=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp294)->f2; goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp294)
== Cyc_Absyn_AssignOp_e){ _LL386: _temp385=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp294)->f1; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp294)->f2; goto _LL382; _LL382: _temp381=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp294)->f3; goto _LL313;} else{ goto _LL314;} _LL314: if(*(( int*) _temp294)
== Cyc_Absyn_NoInstantiate_e){ _LL388: _temp387=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp294)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp294)
== Cyc_Absyn_Instantiate_e){ _LL392: _temp391=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp294)->f1; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp294)->f2; goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp294)
== Cyc_Absyn_Cast_e){ _LL396: _temp395=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp294)->f1; goto _LL394; _LL394: _temp393=(( struct Cyc_Absyn_Cast_e_struct*)
_temp294)->f2; goto _LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp294)
== Cyc_Absyn_Address_e){ _LL398: _temp397=(( struct Cyc_Absyn_Address_e_struct*)
_temp294)->f1; goto _LL321;} else{ goto _LL322;} _LL322: if(*(( int*) _temp294)
== Cyc_Absyn_Sizeofexp_e){ _LL400: _temp399=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp294)->f1; goto _LL323;} else{ goto _LL324;} _LL324: if(*(( int*) _temp294)
== Cyc_Absyn_Deref_e){ _LL402: _temp401=(( struct Cyc_Absyn_Deref_e_struct*)
_temp294)->f1; goto _LL325;} else{ goto _LL326;} _LL326: if(*(( int*) _temp294)
== Cyc_Absyn_StructMember_e){ _LL406: _temp405=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp294)->f1; goto _LL404; _LL404: _temp403=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp294)->f2; goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp294)
== Cyc_Absyn_StructArrow_e){ _LL410: _temp409=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp294)->f1; goto _LL408; _LL408: _temp407=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp294)->f2; goto _LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp294)
== Cyc_Absyn_Increment_e){ _LL414: _temp413=(( struct Cyc_Absyn_Increment_e_struct*)
_temp294)->f1; goto _LL412; _LL412: _temp411=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp294)->f2; goto _LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp294)
== Cyc_Absyn_Sizeoftyp_e){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp294)->f1; goto _LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp294)
== Cyc_Absyn_Offsetof_e){ _LL420: _temp419=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp294)->f1; goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp294)->f2; goto _LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp294)
== Cyc_Absyn_Enum_e){ _LL426: _temp425=(( struct Cyc_Absyn_Enum_e_struct*)
_temp294)->f1; goto _LL424; _LL424: _temp423=(( struct Cyc_Absyn_Enum_e_struct*)
_temp294)->f2; goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_Enum_e_struct*)
_temp294)->f3; goto _LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp294)
== Cyc_Absyn_UnresolvedMem_e){ _LL430: _temp429=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp294)->f1; goto _LL428; _LL428: _temp427=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp294)->f2; goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL297:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)(
varmap, _temp347)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( varmap,
_temp347))->r));} return; _LL299: return; _LL301: return; _LL303: Cyc_Tovc_apply_varmap(
varmap, _temp361); _temp363= _temp359; goto _LL305; _LL305: for( 0; _temp363 !=
0; _temp363=(( struct Cyc_List_List*) _check_null( _temp363))->tl){ Cyc_Tovc_apply_varmap(
varmap,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp363))->hd);}
return; _LL307: Cyc_Tovc_apply_varmap( varmap, _temp367); _temp375= _temp371;
_temp373= _temp369; goto _LL309; _LL309: _temp379= _temp375; _temp377= _temp373;
goto _LL311; _LL311: _temp385= _temp379; _temp381= _temp377; goto _LL313; _LL313:
Cyc_Tovc_apply_varmap( varmap, _temp381); _temp387= _temp385; goto _LL315;
_LL315: _temp391= _temp387; goto _LL317; _LL317: _temp393= _temp391; goto _LL319;
_LL319: _temp397= _temp393; goto _LL321; _LL321: _temp399= _temp397; goto _LL323;
_LL323: _temp401= _temp399; goto _LL325; _LL325: _temp405= _temp401; goto _LL327;
_LL327: _temp409= _temp405; goto _LL329; _LL329: _temp413= _temp409; goto _LL331;
_LL331: Cyc_Tovc_apply_varmap( varmap, _temp413); return; _LL333: return; _LL335:
return; _LL337: return; _LL339: for( 0; _temp427 != 0; _temp427=(( struct Cyc_List_List*)
_check_null( _temp427))->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp427))->hd)).f2);} return; _LL341:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp431=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp431[ 0]=({ struct
Cyc_Core_Impossible_struct _temp432; _temp432.tag= Cyc_Core_Impossible; _temp432.f1=({
unsigned char* _temp433=( unsigned char*)"bad exp form in Tocv::apply_varmap";
struct _tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 35; _temp434;}); _temp432;}); _temp431;}));
_LL295:;} static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_exp( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s){ void* _temp435=( void*) s->r; struct Cyc_Absyn_Exp*
_temp449; struct Cyc_Absyn_Stmt* _temp451; struct Cyc_Absyn_Stmt* _temp453;
struct Cyc_Absyn_Stmt* _temp455; struct Cyc_Absyn_Stmt* _temp457; struct Cyc_Absyn_Exp*
_temp459; struct Cyc_Absyn_Stmt* _temp461; struct Cyc_Absyn_Decl* _temp463;
_LL437: if( _temp435 ==( void*) Cyc_Absyn_Skip_s){ goto _LL438;} else{ goto
_LL439;} _LL439: if(( unsigned int) _temp435 > 1u?*(( int*) _temp435) == Cyc_Absyn_Exp_s:
0){ _LL450: _temp449=(( struct Cyc_Absyn_Exp_s_struct*) _temp435)->f1; goto
_LL440;} else{ goto _LL441;} _LL441: if(( unsigned int) _temp435 > 1u?*(( int*)
_temp435) == Cyc_Absyn_Seq_s: 0){ _LL454: _temp453=(( struct Cyc_Absyn_Seq_s_struct*)
_temp435)->f1; goto _LL452; _LL452: _temp451=(( struct Cyc_Absyn_Seq_s_struct*)
_temp435)->f2; goto _LL442;} else{ goto _LL443;} _LL443: if(( unsigned int)
_temp435 > 1u?*(( int*) _temp435) == Cyc_Absyn_IfThenElse_s: 0){ _LL460:
_temp459=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp435)->f1; goto _LL458;
_LL458: _temp457=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp435)->f2; goto
_LL456; _LL456: _temp455=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp435)->f3;
goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int) _temp435 > 1u?*((
int*) _temp435) == Cyc_Absyn_Decl_s: 0){ _LL464: _temp463=(( struct Cyc_Absyn_Decl_s_struct*)
_temp435)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_Decl_s_struct*)
_temp435)->f2; goto _LL446;} else{ goto _LL447;} _LL447: goto _LL448; _LL438:
return Cyc_Absyn_true_exp( 0); _LL440: Cyc_Tovc_apply_varmap( env.varmap,
_temp449); return _temp449; _LL442: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp453), Cyc_Tovc_stmt_to_exp( env, _temp451), 0); _LL444: Cyc_Tovc_apply_varmap(
env.varmap, _temp459); return Cyc_Absyn_conditional_exp( _temp459, Cyc_Tovc_stmt_to_exp(
env, _temp457), Cyc_Tovc_stmt_to_exp( env, _temp455), 0); _LL446: { void*
_temp465=( void*) _temp463->r; struct Cyc_Absyn_Vardecl* _temp471; _LL467: if(*((
int*) _temp465) == Cyc_Absyn_Var_d){ _LL472: _temp471=(( struct Cyc_Absyn_Var_d_struct*)
_temp465)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL468: {
struct _tagged_string* _temp473=(* _temp471->name).f2; goto _LL474; _LL474: {
struct _tuple0* _temp475= Cyc_Toc_temp_var(); goto _LL476; _LL476: _temp471->name=
_temp475; env.varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( env.varmap,
_temp473, Cyc_Absyn_var_exp( _temp475, 0)); if( _temp471->initializer != 0){ Cyc_Tovc_apply_varmap(
env.varmap,( struct Cyc_Absyn_Exp*) _check_null( _temp471->initializer));{ void*
_temp477=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp471->initializer))->r;
struct Cyc_List_List* _temp483; struct Cyc_Core_Opt* _temp485; _LL479: if(*((
int*) _temp477) == Cyc_Absyn_UnresolvedMem_e){ _LL486: _temp485=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp477)->f1; goto _LL484; _LL484: _temp483=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp477)->f2; goto _LL480;} else{ goto _LL481;} _LL481: goto _LL482; _LL480: {
struct _tuple0* tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp471->type, _temp471->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp471->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp471->type);
goto _LL478;} _LL482: goto _LL478; _LL478:;}} env.all_locals=({ struct Cyc_List_List*
_temp487=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp487->hd=( void*) _temp471; _temp487->tl= env.all_locals; _temp487;});{
struct Cyc_Absyn_Exp* _temp488= _temp471->initializer; goto _LL489; _LL489:
_temp471->initializer= 0;( void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp463, Cyc_Absyn_new_stmt(( void*)( env.encloser)->r, 0), 0))->r));{ struct
Cyc_Absyn_Exp* _temp490= Cyc_Tovc_stmt_to_exp( env, _temp461); goto _LL491;
_LL491: if( _temp488 != 0){ _temp490= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp471->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp488), 0), _temp490, 0);} return _temp490;}}}} _LL470:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp492=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp492[ 0]=({ struct
Cyc_Core_Impossible_struct _temp493; _temp493.tag= Cyc_Core_Impossible; _temp493.f1=({
unsigned char* _temp494=( unsigned char*)"bad local decl in Tovc::stmt_to_exp";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 36; _temp495;}); _temp493;}); _temp492;}));
_LL466:;} _LL448: return Cyc_Tovc_stmt_to_fun( env, s,( void*) Cyc_Absyn_VoidType);
_LL436:;} static void Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv env, struct
Cyc_Absyn_Stmt* s); static void Cyc_Tovc_exp_to_vc( struct Cyc_Tovc_ToExpEnv env,
struct Cyc_Absyn_Exp* e){ void* _temp496=( void*) e->r; void* _temp544; void*
_temp546; struct _tuple0* _temp548; struct Cyc_Absyn_VarargCallInfo* _temp550;
struct Cyc_List_List* _temp552; struct Cyc_Absyn_Exp* _temp554; struct Cyc_List_List*
_temp556; void* _temp558; struct Cyc_Absyn_Exp* _temp560; struct Cyc_Absyn_Exp*
_temp562; struct Cyc_Absyn_Exp* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_Absyn_Exp* _temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp*
_temp572; struct Cyc_Absyn_Exp* _temp574; struct Cyc_Core_Opt* _temp576; struct
Cyc_Absyn_Exp* _temp578; struct Cyc_Absyn_Exp* _temp580; struct Cyc_List_List*
_temp582; struct Cyc_Absyn_Exp* _temp584; struct Cyc_Absyn_Exp* _temp586; void*
_temp588; struct Cyc_Absyn_Exp* _temp590; struct Cyc_Absyn_Exp* _temp592; struct
Cyc_Absyn_Exp* _temp594; struct _tagged_string* _temp596; struct Cyc_Absyn_Exp*
_temp598; struct _tagged_string* _temp600; struct Cyc_Absyn_Exp* _temp602; void*
_temp604; struct Cyc_Absyn_Exp* _temp606; void* _temp608; struct _tagged_string*
_temp610; void* _temp612; struct Cyc_Absyn_Enumfield* _temp614; struct Cyc_Absyn_Enumdecl*
_temp616; struct _tuple0* _temp618; struct Cyc_List_List* _temp620; struct Cyc_Core_Opt*
_temp622; struct Cyc_Absyn_Stmt* _temp624; _LL498: if(*(( int*) _temp496) == Cyc_Absyn_Const_e){
_LL545: _temp544=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp496)->f1;
goto _LL499;} else{ goto _LL500;} _LL500: if(*(( int*) _temp496) == Cyc_Absyn_Var_e){
_LL549: _temp548=(( struct Cyc_Absyn_Var_e_struct*) _temp496)->f1; goto _LL547;
_LL547: _temp546=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp496)->f2; goto
_LL501;} else{ goto _LL502;} _LL502: if(*(( int*) _temp496) == Cyc_Absyn_FnCall_e){
_LL555: _temp554=(( struct Cyc_Absyn_FnCall_e_struct*) _temp496)->f1; goto
_LL553; _LL553: _temp552=(( struct Cyc_Absyn_FnCall_e_struct*) _temp496)->f2;
goto _LL551; _LL551: _temp550=(( struct Cyc_Absyn_FnCall_e_struct*) _temp496)->f3;
goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp496) == Cyc_Absyn_Primop_e){
_LL559: _temp558=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp496)->f1;
goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_Primop_e_struct*) _temp496)->f2;
goto _LL505;} else{ goto _LL506;} _LL506: if(*(( int*) _temp496) == Cyc_Absyn_Conditional_e){
_LL565: _temp564=(( struct Cyc_Absyn_Conditional_e_struct*) _temp496)->f1; goto
_LL563; _LL563: _temp562=(( struct Cyc_Absyn_Conditional_e_struct*) _temp496)->f2;
goto _LL561; _LL561: _temp560=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp496)->f3; goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp496)
== Cyc_Absyn_Subscript_e){ _LL569: _temp568=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp496)->f1; goto _LL567; _LL567: _temp566=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp496)->f2; goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp496)
== Cyc_Absyn_SeqExp_e){ _LL573: _temp572=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp496)->f1; goto _LL571; _LL571: _temp570=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp496)->f2; goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp496)
== Cyc_Absyn_AssignOp_e){ _LL579: _temp578=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp496)->f1; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp496)->f2; goto _LL575; _LL575: _temp574=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp496)->f3; goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp496)
== Cyc_Absyn_NoInstantiate_e){ _LL581: _temp580=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp496)->f1; goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp496)
== Cyc_Absyn_Instantiate_e){ _LL585: _temp584=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp496)->f1; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp496)->f2; goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp496)
== Cyc_Absyn_Cast_e){ _LL589: _temp588=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp496)->f1; goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_Cast_e_struct*)
_temp496)->f2; goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp496)
== Cyc_Absyn_Address_e){ _LL591: _temp590=(( struct Cyc_Absyn_Address_e_struct*)
_temp496)->f1; goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp496)
== Cyc_Absyn_Sizeofexp_e){ _LL593: _temp592=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp496)->f1; goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp496)
== Cyc_Absyn_Deref_e){ _LL595: _temp594=(( struct Cyc_Absyn_Deref_e_struct*)
_temp496)->f1; goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp496)
== Cyc_Absyn_StructMember_e){ _LL599: _temp598=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp496)->f1; goto _LL597; _LL597: _temp596=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp496)->f2; goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp496)
== Cyc_Absyn_StructArrow_e){ _LL603: _temp602=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp496)->f1; goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp496)->f2; goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp496)
== Cyc_Absyn_Increment_e){ _LL607: _temp606=(( struct Cyc_Absyn_Increment_e_struct*)
_temp496)->f1; goto _LL605; _LL605: _temp604=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp496)->f2; goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp496)
== Cyc_Absyn_Sizeoftyp_e){ _LL609: _temp608=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp496)->f1; goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp496)
== Cyc_Absyn_Offsetof_e){ _LL613: _temp612=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp496)->f1; goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp496)->f2; goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp496)
== Cyc_Absyn_Enum_e){ _LL619: _temp618=(( struct Cyc_Absyn_Enum_e_struct*)
_temp496)->f1; goto _LL617; _LL617: _temp616=(( struct Cyc_Absyn_Enum_e_struct*)
_temp496)->f2; goto _LL615; _LL615: _temp614=(( struct Cyc_Absyn_Enum_e_struct*)
_temp496)->f3; goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp496)
== Cyc_Absyn_UnresolvedMem_e){ _LL623: _temp622=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp496)->f1; goto _LL621; _LL621: _temp620=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp496)->f2; goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp496)
== Cyc_Absyn_StmtExp_e){ _LL625: _temp624=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp496)->f1; goto _LL541;} else{ goto _LL542;} _LL542: goto _LL543; _LL499:
return; _LL501: return; _LL503: Cyc_Tovc_exp_to_vc( env, _temp554); _temp556=
_temp552; goto _LL505; _LL505: for( 0; _temp556 != 0; _temp556=(( struct Cyc_List_List*)
_check_null( _temp556))->tl){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp556))->hd);} return; _LL507: Cyc_Tovc_exp_to_vc(
env, _temp560); _temp568= _temp564; _temp566= _temp562; goto _LL509; _LL509:
_temp572= _temp568; _temp570= _temp566; goto _LL511; _LL511: _temp578= _temp572;
_temp574= _temp570; goto _LL513; _LL513: Cyc_Tovc_exp_to_vc( env, _temp574);
_temp580= _temp578; goto _LL515; _LL515: _temp584= _temp580; goto _LL517; _LL517:
_temp586= _temp584; goto _LL519; _LL519: _temp590= _temp586; goto _LL521; _LL521:
_temp592= _temp590; goto _LL523; _LL523: _temp594= _temp592; goto _LL525; _LL525:
_temp598= _temp594; goto _LL527; _LL527: _temp602= _temp598; goto _LL529; _LL529:
_temp606= _temp602; goto _LL531; _LL531: Cyc_Tovc_exp_to_vc( env, _temp606);
return; _LL533: return; _LL535: return; _LL537: return; _LL539: for( 0; _temp620
!= 0; _temp620=(( struct Cyc_List_List*) _check_null( _temp620))->tl){ Cyc_Tovc_exp_to_vc(
env,(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp620))->hd)).f2);}
return; _LL541: Cyc_Tovc_stmt_to_vc( env, _temp624);( void*)( e->r=( void*)((
void*)( Cyc_Tovc_stmt_to_exp( env, _temp624))->r)); return; _LL543:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp626=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp626[ 0]=({ struct
Cyc_Core_Impossible_struct _temp627; _temp627.tag= Cyc_Core_Impossible; _temp627.f1=({
unsigned char* _temp628=( unsigned char*)"bad exp form after xlation to C";
struct _tagged_string _temp629; _temp629.curr= _temp628; _temp629.base= _temp628;
_temp629.last_plus_one= _temp628 + 32; _temp629;}); _temp627;}); _temp626;}));
_LL497:;} static void Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv env, struct
Cyc_Absyn_Stmt* s){ while( 1) { env.encloser= s;{ void* _temp630=( void*) s->r;
struct Cyc_Absyn_Stmt* _temp662; struct Cyc_Absyn_Stmt* _temp664; struct Cyc_Absyn_Stmt*
_temp666; struct _tagged_string* _temp668; struct Cyc_Absyn_Stmt* _temp670;
struct _tagged_string* _temp672; struct Cyc_Absyn_Exp* _temp674; struct Cyc_Absyn_Exp*
_temp676; struct Cyc_Absyn_Stmt* _temp678; struct Cyc_Absyn_Stmt* _temp680;
struct Cyc_Absyn_Stmt* _temp682; struct Cyc_Absyn_Stmt* _temp684; struct Cyc_Absyn_Exp*
_temp686; struct Cyc_Absyn_Stmt* _temp688; struct _tuple2 _temp690; struct Cyc_Absyn_Stmt*
_temp692; struct Cyc_Absyn_Exp* _temp694; struct Cyc_Absyn_Stmt* _temp696;
struct _tuple2 _temp698; struct Cyc_Absyn_Stmt* _temp700; struct Cyc_Absyn_Exp*
_temp702; struct _tuple2 _temp704; struct Cyc_Absyn_Stmt* _temp706; struct Cyc_Absyn_Exp*
_temp708; struct Cyc_Absyn_Exp* _temp710; struct _tuple2 _temp712; struct Cyc_Absyn_Stmt*
_temp714; struct Cyc_Absyn_Exp* _temp716; struct Cyc_Absyn_Stmt* _temp718;
struct Cyc_List_List* _temp720; struct Cyc_Absyn_Exp* _temp722; struct Cyc_Absyn_Stmt*
_temp724; struct Cyc_Absyn_Decl* _temp726; _LL632: if( _temp630 ==( void*) Cyc_Absyn_Skip_s){
goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int) _temp630 > 1u?*((
int*) _temp630) == Cyc_Absyn_Break_s: 0){ _LL663: _temp662=(( struct Cyc_Absyn_Break_s_struct*)
_temp630)->f1; goto _LL635;} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Continue_s: 0){ _LL665: _temp664=((
struct Cyc_Absyn_Continue_s_struct*) _temp630)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: if(( unsigned int) _temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Goto_s:
0){ _LL669: _temp668=(( struct Cyc_Absyn_Goto_s_struct*) _temp630)->f1; goto
_LL667; _LL667: _temp666=(( struct Cyc_Absyn_Goto_s_struct*) _temp630)->f2; goto
_LL639;} else{ goto _LL640;} _LL640: if(( unsigned int) _temp630 > 1u?*(( int*)
_temp630) == Cyc_Absyn_Label_s: 0){ _LL673: _temp672=(( struct Cyc_Absyn_Label_s_struct*)
_temp630)->f1; goto _LL671; _LL671: _temp670=(( struct Cyc_Absyn_Label_s_struct*)
_temp630)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Return_s: 0){ _LL675: _temp674=((
struct Cyc_Absyn_Return_s_struct*) _temp630)->f1; goto _LL643;} else{ goto
_LL644;} _LL644: if(( unsigned int) _temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Exp_s:
0){ _LL677: _temp676=(( struct Cyc_Absyn_Exp_s_struct*) _temp630)->f1; goto
_LL645;} else{ goto _LL646;} _LL646: if(( unsigned int) _temp630 > 1u?*(( int*)
_temp630) == Cyc_Absyn_Seq_s: 0){ _LL681: _temp680=(( struct Cyc_Absyn_Seq_s_struct*)
_temp630)->f1; goto _LL679; _LL679: _temp678=(( struct Cyc_Absyn_Seq_s_struct*)
_temp630)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int)
_temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_IfThenElse_s: 0){ _LL687:
_temp686=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp630)->f1; goto _LL685;
_LL685: _temp684=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp630)->f2; goto
_LL683; _LL683: _temp682=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp630)->f3;
goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int) _temp630 > 1u?*((
int*) _temp630) == Cyc_Absyn_While_s: 0){ _LL691: _temp690=(( struct Cyc_Absyn_While_s_struct*)
_temp630)->f1; _LL695: _temp694= _temp690.f1; goto _LL693; _LL693: _temp692=
_temp690.f2; goto _LL689; _LL689: _temp688=(( struct Cyc_Absyn_While_s_struct*)
_temp630)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int)
_temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_For_s: 0){ _LL711: _temp710=((
struct Cyc_Absyn_For_s_struct*) _temp630)->f1; goto _LL705; _LL705: _temp704=((
struct Cyc_Absyn_For_s_struct*) _temp630)->f2; _LL709: _temp708= _temp704.f1;
goto _LL707; _LL707: _temp706= _temp704.f2; goto _LL699; _LL699: _temp698=((
struct Cyc_Absyn_For_s_struct*) _temp630)->f3; _LL703: _temp702= _temp698.f1;
goto _LL701; _LL701: _temp700= _temp698.f2; goto _LL697; _LL697: _temp696=((
struct Cyc_Absyn_For_s_struct*) _temp630)->f4; goto _LL653;} else{ goto _LL654;}
_LL654: if(( unsigned int) _temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Do_s:
0){ _LL719: _temp718=(( struct Cyc_Absyn_Do_s_struct*) _temp630)->f1; goto
_LL713; _LL713: _temp712=(( struct Cyc_Absyn_Do_s_struct*) _temp630)->f2; _LL717:
_temp716= _temp712.f1; goto _LL715; _LL715: _temp714= _temp712.f2; goto _LL655;}
else{ goto _LL656;} _LL656: if(( unsigned int) _temp630 > 1u?*(( int*) _temp630)
== Cyc_Absyn_Switch_s: 0){ _LL723: _temp722=(( struct Cyc_Absyn_Switch_s_struct*)
_temp630)->f1; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_Switch_s_struct*)
_temp630)->f2; goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int)
_temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Decl_s: 0){ _LL727: _temp726=((
struct Cyc_Absyn_Decl_s_struct*) _temp630)->f1; goto _LL725; _LL725: _temp724=((
struct Cyc_Absyn_Decl_s_struct*) _temp630)->f2; goto _LL659;} else{ goto _LL660;}
_LL660: goto _LL661; _LL633: return; _LL635: return; _LL637: return; _LL639:
return; _LL641: s= _temp670; continue; _LL643: if( _temp674 == 0){ return;}
_temp676=( struct Cyc_Absyn_Exp*) _check_null( _temp674); goto _LL645; _LL645:
Cyc_Tovc_exp_to_vc( env, _temp676); return; _LL647: Cyc_Tovc_stmt_to_vc( env,
_temp680); s= _temp678; continue; _LL649: Cyc_Tovc_exp_to_vc( env, _temp686);
Cyc_Tovc_stmt_to_vc( env, _temp684); s= _temp682; continue; _LL651: Cyc_Tovc_exp_to_vc(
env, _temp694); s= _temp688; continue; _LL653: Cyc_Tovc_exp_to_vc( env, _temp710);
Cyc_Tovc_exp_to_vc( env, _temp708); Cyc_Tovc_exp_to_vc( env, _temp702); s=
_temp696; continue; _LL655: Cyc_Tovc_exp_to_vc( env, _temp716); s= _temp718;
continue; _LL657: Cyc_Tovc_exp_to_vc( env, _temp722); for( 0; _temp720 != 0;
_temp720=(( struct Cyc_List_List*) _check_null( _temp720))->tl){ Cyc_Tovc_stmt_to_vc(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp720))->hd)->body);} return; _LL659:{ void* _temp728=( void*) _temp726->r;
struct Cyc_Absyn_Vardecl* _temp734; _LL730: if(*(( int*) _temp728) == Cyc_Absyn_Var_d){
_LL735: _temp734=(( struct Cyc_Absyn_Var_d_struct*) _temp728)->f1; goto _LL731;}
else{ goto _LL732;} _LL732: goto _LL733; _LL731:{ void* _temp736= Cyc_Tcutil_compress((
void*) _temp734->type); struct Cyc_Absyn_Exp* _temp742; struct Cyc_Absyn_Tqual
_temp744; void* _temp746; _LL738: if(( unsigned int) _temp736 > 4u?*(( int*)
_temp736) == Cyc_Absyn_ArrayType: 0){ _LL747: _temp746=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp736)->f1; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_ArrayType_struct*)
_temp736)->f2; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_ArrayType_struct*)
_temp736)->f3; goto _LL739;} else{ goto _LL740;} _LL740: goto _LL741; _LL739:
if( _temp734->initializer != 0){ void* _temp748=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp734->initializer))->r; struct Cyc_List_List* _temp754; struct
Cyc_Core_Opt* _temp756; _LL750: if(*(( int*) _temp748) == Cyc_Absyn_UnresolvedMem_e){
_LL757: _temp756=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp748)->f1;
goto _LL755; _LL755: _temp754=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp748)->f2; goto _LL751;} else{ goto _LL752;} _LL752: goto _LL753; _LL751:
_temp734->initializer= 0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp(
_temp734->name, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_new_stmt(( void*)
_temp724->r, 0);{ unsigned int i= 0; for( 0; _temp754 != 0;( _temp754=(( struct
Cyc_List_List*) _check_null( _temp754))->tl, ++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp( arr, Cyc_Absyn_uint_exp( i, 0), 0),(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp754))->hd)).f2, 0),
0), s3, 0);}}( void*)( _temp724->r=( void*)(( void*) s3->r)); goto _LL749;}
_LL753:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp758=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp758[ 0]=({ struct Cyc_Core_Impossible_struct _temp759; _temp759.tag= Cyc_Core_Impossible;
_temp759.f1=({ unsigned char* _temp760=( unsigned char*)"array type bad initializer";
struct _tagged_string _temp761; _temp761.curr= _temp760; _temp761.base= _temp760;
_temp761.last_plus_one= _temp760 + 27; _temp761;}); _temp759;}); _temp758;}));
_LL749:;} goto _LL737; _LL741: goto _LL737; _LL737:;} env.all_locals=({ struct
Cyc_List_List* _temp762=( struct Cyc_List_List*) _region_malloc( env.rgn,
sizeof( struct Cyc_List_List)); _temp762->hd=( void*) _temp734; _temp762->tl=
env.all_locals; _temp762;}); if( _temp734->initializer != 0){ Cyc_Tovc_exp_to_vc(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp734->initializer));} s= _temp724;
continue; _LL733: goto _LL729; _LL729:;} goto _LL661; _LL661:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp763=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp763[ 0]=({ struct
Cyc_Core_Impossible_struct _temp764; _temp764.tag= Cyc_Core_Impossible; _temp764.f1=({
unsigned char* _temp765=( unsigned char*)"bad stmt after xlation to C"; struct
_tagged_string _temp766; _temp766.curr= _temp765; _temp766.base= _temp765;
_temp766.last_plus_one= _temp765 + 28; _temp766;}); _temp764;}); _temp763;}));
_LL631:;}}} struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* old_ds){
struct Cyc_List_List* _temp767= 0; goto _LL768; _LL768: for( 0; old_ds != 0;
old_ds=(( struct Cyc_List_List*) _check_null( old_ds))->tl){ struct Cyc_Absyn_Decl*
_temp769=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( old_ds))->hd;
goto _LL770; _LL770: { void* _temp771=( void*) _temp769->r; struct Cyc_Absyn_Fndecl*
_temp797; struct Cyc_Absyn_Vardecl* _temp799; int _temp801; struct Cyc_Absyn_Exp*
_temp803; struct Cyc_Core_Opt* _temp805; struct Cyc_Core_Opt* _temp807; struct
Cyc_Absyn_Pat* _temp809; struct Cyc_List_List* _temp811; struct Cyc_Absyn_Structdecl*
_temp813; struct Cyc_Absyn_Uniondecl* _temp815; struct Cyc_Absyn_Tuniondecl*
_temp817; struct Cyc_Absyn_Enumdecl* _temp819; struct Cyc_Absyn_Typedefdecl*
_temp821; struct Cyc_List_List* _temp823; struct _tagged_string* _temp825;
struct Cyc_List_List* _temp827; struct _tuple0* _temp829; struct Cyc_List_List*
_temp831; _LL773: if(*(( int*) _temp771) == Cyc_Absyn_Fn_d){ _LL798: _temp797=((
struct Cyc_Absyn_Fn_d_struct*) _temp771)->f1; goto _LL774;} else{ goto _LL775;}
_LL775: if(*(( int*) _temp771) == Cyc_Absyn_Var_d){ _LL800: _temp799=(( struct
Cyc_Absyn_Var_d_struct*) _temp771)->f1; goto _LL776;} else{ goto _LL777;} _LL777:
if(*(( int*) _temp771) == Cyc_Absyn_Let_d){ _LL810: _temp809=(( struct Cyc_Absyn_Let_d_struct*)
_temp771)->f1; goto _LL808; _LL808: _temp807=(( struct Cyc_Absyn_Let_d_struct*)
_temp771)->f2; goto _LL806; _LL806: _temp805=(( struct Cyc_Absyn_Let_d_struct*)
_temp771)->f3; goto _LL804; _LL804: _temp803=(( struct Cyc_Absyn_Let_d_struct*)
_temp771)->f4; goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_Let_d_struct*)
_temp771)->f5; goto _LL778;} else{ goto _LL779;} _LL779: if(*(( int*) _temp771)
== Cyc_Absyn_Letv_d){ _LL812: _temp811=(( struct Cyc_Absyn_Letv_d_struct*)
_temp771)->f1; goto _LL780;} else{ goto _LL781;} _LL781: if(*(( int*) _temp771)
== Cyc_Absyn_Struct_d){ _LL814: _temp813=(( struct Cyc_Absyn_Struct_d_struct*)
_temp771)->f1; goto _LL782;} else{ goto _LL783;} _LL783: if(*(( int*) _temp771)
== Cyc_Absyn_Union_d){ _LL816: _temp815=(( struct Cyc_Absyn_Union_d_struct*)
_temp771)->f1; goto _LL784;} else{ goto _LL785;} _LL785: if(*(( int*) _temp771)
== Cyc_Absyn_Tunion_d){ _LL818: _temp817=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp771)->f1; goto _LL786;} else{ goto _LL787;} _LL787: if(*(( int*) _temp771)
== Cyc_Absyn_Enum_d){ _LL820: _temp819=(( struct Cyc_Absyn_Enum_d_struct*)
_temp771)->f1; goto _LL788;} else{ goto _LL789;} _LL789: if(*(( int*) _temp771)
== Cyc_Absyn_Typedef_d){ _LL822: _temp821=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp771)->f1; goto _LL790;} else{ goto _LL791;} _LL791: if(*(( int*) _temp771)
== Cyc_Absyn_Namespace_d){ _LL826: _temp825=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp771)->f1; goto _LL824; _LL824: _temp823=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp771)->f2; goto _LL792;} else{ goto _LL793;} _LL793: if(*(( int*) _temp771)
== Cyc_Absyn_Using_d){ _LL830: _temp829=(( struct Cyc_Absyn_Using_d_struct*)
_temp771)->f1; goto _LL828; _LL828: _temp827=(( struct Cyc_Absyn_Using_d_struct*)
_temp771)->f2; goto _LL794;} else{ goto _LL795;} _LL795: if(*(( int*) _temp771)
== Cyc_Absyn_ExternC_d){ _LL832: _temp831=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp771)->f1; goto _LL796;} else{ goto _LL772;} _LL774:{ struct _RegionHandle
_temp833= _new_region(); struct _RegionHandle* rgn=& _temp833; _push_region( rgn);{
struct Cyc_List_List** _temp835=({ struct Cyc_List_List** _temp834=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp834[ 0]= 0; _temp834;});
goto _LL836; _LL836: Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp837;
_temp837.rgn= rgn; _temp837.all_locals=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* src)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp797->param_vardecls))->v); _temp837.varmap=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle*, int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_rempty)( rgn, Cyc_String_zstrptrcmp);
_temp837.encloser= _temp797->body; _temp837.gen_ds= _temp835; _temp837;}),
_temp797->body); _temp767=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)(* _temp835, _temp767);}; _pop_region(&
_temp833);} goto _LL776; _LL776: goto _LL778; _LL778: goto _LL780; _LL780: goto
_LL782; _LL782: goto _LL784; _LL784: goto _LL786; _LL786: goto _LL788; _LL788:
goto _LL790; _LL790: _temp767=({ struct Cyc_List_List* _temp838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp838->hd=( void*) _temp769;
_temp838->tl= _temp767; _temp838;}); goto _LL772; _LL792: _temp827= _temp823;
goto _LL794; _LL794: _temp831= _temp827; goto _LL796; _LL796:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp839=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp839[ 0]=({ struct
Cyc_Core_Impossible_struct _temp840; _temp840.tag= Cyc_Core_Impossible; _temp840.f1=({
unsigned char* _temp841=( unsigned char*)"nested translation unit after translation to C";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 47; _temp842;}); _temp840;}); _temp839;}));
_LL772:;}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp767);}