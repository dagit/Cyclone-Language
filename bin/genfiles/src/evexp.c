 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
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
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct
Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct
Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char
Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern int
Cyc_Evexp_okay_szofarg( void* t); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_string)({ struct _tagged_string _temp0= msg; xprintf("Exp_err: %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);})));} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); void* Cyc_Evexp_promote_const( void* cn){ void* _temp1=
cn; unsigned char _temp9; void* _temp11; short _temp13; void* _temp15; _LL3: if((
unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Absyn_Char_c: 0){ _LL12:
_temp11=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1)->f1; goto _LL10;
_LL10: _temp9=(( struct Cyc_Absyn_Char_c_struct*) _temp1)->f2; goto _LL4;} else{
goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Absyn_Short_c:
0){ _LL16: _temp15=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1)->f1;
goto _LL14; _LL14: _temp13=(( struct Cyc_Absyn_Short_c_struct*) _temp1)->f2;
goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL4: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp17=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp17[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp18; _temp18.tag= Cyc_Absyn_Int_c;
_temp18.f1=( void*) _temp11; _temp18.f2=( int) _temp9; _temp18;}); _temp17;});
_LL6: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp19=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp19[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp20; _temp20.tag= Cyc_Absyn_Int_c; _temp20.f1=( void*) _temp15; _temp20.f2=(
int) _temp13; _temp20;}); _temp19;}); _LL8: return cn; _LL2:;} unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp21= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp27; void* _temp29; _LL23: if(( unsigned int) _temp21 > 1u?*(( int*)
_temp21) == Cyc_Absyn_Int_c: 0){ _LL30: _temp29=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp21)->f1; goto _LL28; _LL28: _temp27=(( struct Cyc_Absyn_Int_c_struct*)
_temp21)->f2; goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL24: return(
unsigned int) _temp27; _LL26: Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp31=( unsigned char*)"expecting unsigned int"; struct _tagged_string _temp32;
_temp32.curr= _temp31; _temp32.base= _temp31; _temp32.last_plus_one= _temp31 +
23; _temp32;})); return 0u; _LL22:;} int Cyc_Evexp_eval_const_bool_exp( struct
Cyc_Absyn_Exp* e){ void* _temp33= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp43; void* _temp45; long long _temp47; void* _temp49; _LL35: if((
unsigned int) _temp33 > 1u?*(( int*) _temp33) == Cyc_Absyn_Int_c: 0){ _LL46:
_temp45=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f1; goto _LL44;
_LL44: _temp43=(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f2; goto _LL36;}
else{ goto _LL37;} _LL37: if(( unsigned int) _temp33 > 1u?*(( int*) _temp33) ==
Cyc_Absyn_LongLong_c: 0){ _LL50: _temp49=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp33)->f1; goto _LL48; _LL48: _temp47=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp33)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if( _temp33 ==( void*) Cyc_Absyn_Null_c){
goto _LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL36: return _temp43 != 0;
_LL38: return _temp47 != 0; _LL40: return 0; _LL42: Cyc_Evexp_exp_err( e->loc,({
unsigned char* _temp51=( unsigned char*)"expecting bool"; struct _tagged_string
_temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 15; _temp52;})); return 0; _LL34:;} struct _tuple3{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; int Cyc_Evexp_okay_szofarg( void* t){ void* _temp53= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp103; struct Cyc_Core_Opt* _temp105; int _temp107;
struct Cyc_Core_Opt* _temp109; struct Cyc_Core_Opt* _temp111; struct Cyc_Core_Opt
_temp113; void* _temp114; struct Cyc_Core_Opt* _temp116; int _temp118; struct
Cyc_Core_Opt* _temp120; struct Cyc_Core_Opt* _temp122; struct Cyc_Absyn_TunionInfo
_temp124; struct Cyc_Absyn_TunionFieldInfo _temp126; struct Cyc_List_List*
_temp128; void* _temp130; struct Cyc_Absyn_Tunionfield* _temp132; struct Cyc_Absyn_Tuniondecl*
_temp134; struct Cyc_List_List* _temp136; struct Cyc_Absyn_TunionFieldInfo
_temp138; struct Cyc_Absyn_PtrInfo _temp140; struct Cyc_Absyn_Conref* _temp142;
struct Cyc_Absyn_Tqual _temp144; struct Cyc_Absyn_Conref* _temp146; void*
_temp148; void* _temp150; void* _temp152; void* _temp154; struct Cyc_Absyn_Exp*
_temp156; struct Cyc_Absyn_Tqual _temp158; void* _temp160; struct Cyc_Absyn_FnInfo
_temp162; void* _temp164; struct Cyc_Absyn_Structdecl** _temp166; struct Cyc_List_List*
_temp168; struct _tuple0* _temp170; struct Cyc_List_List* _temp172; struct Cyc_Absyn_Uniondecl**
_temp174; struct Cyc_List_List* _temp176; struct _tuple0* _temp178; struct Cyc_List_List*
_temp180; struct Cyc_Absyn_Enumdecl* _temp182; struct _tuple0* _temp184; struct
Cyc_List_List* _temp186; void* _temp188; struct Cyc_Core_Opt* _temp190; struct
Cyc_List_List* _temp192; struct _tuple0* _temp194; _LL55: if( _temp53 ==( void*)
Cyc_Absyn_VoidType){ goto _LL56;} else{ goto _LL57;} _LL57: if(( unsigned int)
_temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_VarType: 0){ _LL104: _temp103=((
struct Cyc_Absyn_VarType_struct*) _temp53)->f1; goto _LL58;} else{ goto _LL59;}
_LL59: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_Evar: 0){
_LL112: _temp111=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f1; if( _temp111 ==
0){ goto _LL61;} else{ _temp113=* _temp111; _LL115: _temp114=( void*) _temp113.v;
if( _temp114 ==( void*) Cyc_Absyn_BoxKind){ goto _LL110;} else{ goto _LL61;}}
_LL110: _temp109=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f2; goto _LL108;
_LL108: _temp107=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f3; goto _LL106;
_LL106: _temp105=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f4; goto _LL60;}
else{ goto _LL61;} _LL61: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) ==
Cyc_Absyn_Evar: 0){ _LL123: _temp122=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f1;
goto _LL121; _LL121: _temp120=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f2;
goto _LL119; _LL119: _temp118=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f3;
goto _LL117; _LL117: _temp116=(( struct Cyc_Absyn_Evar_struct*) _temp53)->f4;
goto _LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_TunionType: 0){ _LL125: _temp124=(( struct Cyc_Absyn_TunionType_struct*)
_temp53)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_TunionFieldType: 0){ _LL127: _temp126=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp53)->f1; _LL131: _temp130=( void*)
_temp126.field_info; if(*(( int*) _temp130) == Cyc_Absyn_KnownTunionfield){
_LL135: _temp134=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp130)->f1;
goto _LL133; _LL133: _temp132=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp130)->f2; goto _LL129;} else{ goto _LL67;} _LL129: _temp128= _temp126.targs;
goto _LL66;} else{ goto _LL67;} _LL67: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_TupleType: 0){ _LL137: _temp136=(( struct Cyc_Absyn_TupleType_struct*)
_temp53)->f1; goto _LL68;} else{ goto _LL69;} _LL69: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_TunionFieldType: 0){ _LL139: _temp138=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp53)->f1; goto _LL70;} else{ goto
_LL71;} _LL71: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_PointerType:
0){ _LL141: _temp140=(( struct Cyc_Absyn_PointerType_struct*) _temp53)->f1;
_LL151: _temp150=( void*) _temp140.elt_typ; goto _LL149; _LL149: _temp148=( void*)
_temp140.rgn_typ; goto _LL147; _LL147: _temp146= _temp140.nullable; goto _LL145;
_LL145: _temp144= _temp140.tq; goto _LL143; _LL143: _temp142= _temp140.bounds;
goto _LL72;} else{ goto _LL73;} _LL73: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_IntType: 0){ _LL155: _temp154=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp53)->f1; goto _LL153; _LL153: _temp152=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp53)->f2; goto _LL74;} else{ goto _LL75;} _LL75: if( _temp53 ==( void*) Cyc_Absyn_FloatType){
goto _LL76;} else{ goto _LL77;} _LL77: if( _temp53 ==( void*) Cyc_Absyn_DoubleType){
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_ArrayType: 0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp53)->f1; goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_ArrayType_struct*)
_temp53)->f2; goto _LL157; _LL157: _temp156=(( struct Cyc_Absyn_ArrayType_struct*)
_temp53)->f3; goto _LL80;} else{ goto _LL81;} _LL81: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_FnType: 0){ _LL163: _temp162=(( struct Cyc_Absyn_FnType_struct*)
_temp53)->f1; goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_RgnHandleType: 0){ _LL165: _temp164=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp53)->f1; goto _LL84;} else{ goto
_LL85;} _LL85: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_StructType:
0){ _LL171: _temp170=(( struct Cyc_Absyn_StructType_struct*) _temp53)->f1; goto
_LL169; _LL169: _temp168=(( struct Cyc_Absyn_StructType_struct*) _temp53)->f2;
goto _LL167; _LL167: _temp166=(( struct Cyc_Absyn_StructType_struct*) _temp53)->f3;
goto _LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_AnonStructType: 0){ _LL173: _temp172=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp53)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_UnionType: 0){ _LL179: _temp178=(( struct
Cyc_Absyn_UnionType_struct*) _temp53)->f1; goto _LL177; _LL177: _temp176=((
struct Cyc_Absyn_UnionType_struct*) _temp53)->f2; goto _LL175; _LL175: _temp174=((
struct Cyc_Absyn_UnionType_struct*) _temp53)->f3; goto _LL90;} else{ goto _LL91;}
_LL91: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_AnonUnionType:
0){ _LL181: _temp180=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp53)->f1;
goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_EnumType: 0){ _LL185: _temp184=(( struct Cyc_Absyn_EnumType_struct*)
_temp53)->f1; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_EnumType_struct*)
_temp53)->f2; goto _LL94;} else{ goto _LL95;} _LL95: if( _temp53 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp53 > 4u?*(( int*)
_temp53) == Cyc_Absyn_JoinEff: 0){ _LL187: _temp186=(( struct Cyc_Absyn_JoinEff_struct*)
_temp53)->f1; goto _LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp53
> 4u?*(( int*) _temp53) == Cyc_Absyn_AccessEff: 0){ _LL189: _temp188=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp53)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: if(( unsigned int) _temp53 > 4u?*(( int*) _temp53) == Cyc_Absyn_TypedefType:
0){ _LL195: _temp194=(( struct Cyc_Absyn_TypedefType_struct*) _temp53)->f1; goto
_LL193; _LL193: _temp192=(( struct Cyc_Absyn_TypedefType_struct*) _temp53)->f2;
goto _LL191; _LL191: _temp190=(( struct Cyc_Absyn_TypedefType_struct*) _temp53)->f3;
goto _LL102;} else{ goto _LL54;} _LL56: return 0; _LL58: { void* _temp196=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp103->kind);
_LL198: if( _temp196 ==( void*) Cyc_Absyn_BoxKind){ goto _LL199;} else{ goto
_LL200;} _LL200: goto _LL201; _LL199: return 1; _LL201: return 0; _LL197:;}
_LL60: return 1; _LL62: return 0; _LL64: return 1; _LL66: _temp136= _temp132->typs;
goto _LL68; _LL68: for( 0; _temp136 != 0; _temp136=(( struct Cyc_List_List*)
_check_null( _temp136))->tl){ if( ! Cyc_Evexp_okay_szofarg((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp136))->hd)).f2)){ return 0;}} return 1;
_LL70: return 0; _LL72: { void* _temp202=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp142))->v; void*
_temp210; void* _temp212; _LL204: if(( unsigned int) _temp202 > 1u?*(( int*)
_temp202) == Cyc_Absyn_Eq_constr: 0){ _LL211: _temp210=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp202)->f1; if( _temp210 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL205;} else{
goto _LL206;}} else{ goto _LL206;} _LL206: if(( unsigned int) _temp202 > 1u?*((
int*) _temp202) == Cyc_Absyn_Eq_constr: 0){ _LL213: _temp212=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp202)->f1; goto _LL207;} else{ goto _LL208;}
_LL208: goto _LL209; _LL205: return 1; _LL207: return 1; _LL209: return 0;
_LL203:;} _LL74: return 1; _LL76: return 1; _LL78: return 1; _LL80: return
_temp156 != 0; _LL82: return 0; _LL84: return 1; _LL86: if( _temp166 == 0){
return(( int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char*
_temp214=( unsigned char*)"szof on unchecked StructType"; struct _tagged_string
_temp215; _temp215.curr= _temp214; _temp215.base= _temp214; _temp215.last_plus_one=
_temp214 + 29; _temp215;}));}{ struct Cyc_Absyn_Structdecl* _temp216=*(( struct
Cyc_Absyn_Structdecl**) _check_null( _temp166)); goto _LL217; _LL217: if(
_temp216->fields == 0){ return 0;} _temp172=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp216->fields))->v; goto _LL88;} _LL88: for( 0;
_temp172 != 0; _temp172=(( struct Cyc_List_List*) _check_null( _temp172))->tl){
if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp172))->hd)->type)){ return 0;}} return 1;
_LL90: if( _temp174 == 0){ return(( int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp218=( unsigned char*)"szof on unchecked UnionType"; struct
_tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 28; _temp219;}));}{ struct Cyc_Absyn_Uniondecl*
_temp220=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp174)); goto _LL221;
_LL221: if( _temp220->fields == 0){ return 0;} _temp180=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp220->fields))->v; goto _LL92;} _LL92:
for( 0; _temp180 != 0; _temp180=(( struct Cyc_List_List*) _check_null( _temp180))->tl){
if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp180))->hd)->type)){ return 0;}} return 1;
_LL94: return 1; _LL96: return 0; _LL98: return 0; _LL100: return 0; _LL102:
return(( int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ struct _tagged_string _temp222= Cyc_Absynpp_typ2string( t);
xprintf("szof typedeftype %.*s", _temp222.last_plus_one - _temp222.curr,
_temp222.curr);})); _LL54:;} struct _tuple4{ void* f1; void* f2; } ; void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple4 _temp224=({ struct _tuple4 _temp223; _temp223.f1= p;
_temp223.f2= cn; _temp223;}); void* _temp238; void* _temp240; void* _temp242;
int _temp244; void* _temp246; void* _temp248; void* _temp250; int _temp252; void*
_temp254; void* _temp256; void* _temp258; int _temp260; void* _temp262; void*
_temp264; void* _temp266; void* _temp268; _LL226: _LL241: _temp240= _temp224.f1;
if( _temp240 ==( void*) Cyc_Absyn_Plus){ goto _LL239;} else{ goto _LL228;}
_LL239: _temp238= _temp224.f2; goto _LL227; _LL228: _LL249: _temp248= _temp224.f1;
if( _temp248 ==( void*) Cyc_Absyn_Minus){ goto _LL243;} else{ goto _LL230;}
_LL243: _temp242= _temp224.f2; if(( unsigned int) _temp242 > 1u?*(( int*)
_temp242) == Cyc_Absyn_Int_c: 0){ _LL247: _temp246=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp242)->f1; goto _LL245; _LL245: _temp244=(( struct Cyc_Absyn_Int_c_struct*)
_temp242)->f2; goto _LL229;} else{ goto _LL230;} _LL230: _LL257: _temp256=
_temp224.f1; if( _temp256 ==( void*) Cyc_Absyn_Bitnot){ goto _LL251;} else{ goto
_LL232;} _LL251: _temp250= _temp224.f2; if(( unsigned int) _temp250 > 1u?*(( int*)
_temp250) == Cyc_Absyn_Int_c: 0){ _LL255: _temp254=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp250)->f1; goto _LL253; _LL253: _temp252=(( struct Cyc_Absyn_Int_c_struct*)
_temp250)->f2; goto _LL231;} else{ goto _LL232;} _LL232: _LL265: _temp264=
_temp224.f1; if( _temp264 ==( void*) Cyc_Absyn_Not){ goto _LL259;} else{ goto
_LL234;} _LL259: _temp258= _temp224.f2; if(( unsigned int) _temp258 > 1u?*(( int*)
_temp258) == Cyc_Absyn_Int_c: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp258)->f1; goto _LL261; _LL261: _temp260=(( struct Cyc_Absyn_Int_c_struct*)
_temp258)->f2; goto _LL233;} else{ goto _LL234;} _LL234: _LL269: _temp268=
_temp224.f1; if( _temp268 ==( void*) Cyc_Absyn_Not){ goto _LL267;} else{ goto
_LL236;} _LL267: _temp266= _temp224.f2; if( _temp266 ==( void*) Cyc_Absyn_Null_c){
goto _LL235;} else{ goto _LL236;} _LL236: goto _LL237; _LL227: return cn; _LL229:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp270=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp270[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp271; _temp271.tag= Cyc_Absyn_Int_c; _temp271.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp271.f2= - _temp244; _temp271;}); _temp270;}); _LL231: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp272=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp272[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp273; _temp273.tag= Cyc_Absyn_Int_c; _temp273.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp273.f2= ~ _temp252; _temp273;}); _temp272;}); _LL233: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp274=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp274[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp275; _temp275.tag= Cyc_Absyn_Int_c; _temp275.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp275.f2= _temp260 == 0? 1: 0; _temp275;}); _temp274;}); _LL235: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp276=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp276[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp277; _temp277.tag= Cyc_Absyn_Int_c; _temp277.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp277.f2= 1; _temp277;}); _temp276;}); _LL237: Cyc_Evexp_exp_err( e->loc,({
unsigned char* _temp278=( unsigned char*)"bad constant expression"; struct
_tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 24; _temp279;})); return cn; _LL225:;} struct
_tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp280= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); goto _LL281; _LL281: { void* _temp282= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e2)); goto _LL283; _LL283: { void* s1; void* s2; int
i1; int i2;{ void* _temp284= _temp280; int _temp290; void* _temp292; _LL286: if((
unsigned int) _temp284 > 1u?*(( int*) _temp284) == Cyc_Absyn_Int_c: 0){ _LL293:
_temp292=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp284)->f1; goto _LL291;
_LL291: _temp290=(( struct Cyc_Absyn_Int_c_struct*) _temp284)->f2; goto _LL287;}
else{ goto _LL288;} _LL288: goto _LL289; _LL287: s1= _temp292; i1= _temp290;
goto _LL285; _LL289: Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp294=(
unsigned char*)"bad constant expression"; struct _tagged_string _temp295;
_temp295.curr= _temp294; _temp295.base= _temp294; _temp295.last_plus_one=
_temp294 + 24; _temp295;})); return _temp280; _LL285:;}{ void* _temp296=
_temp282; int _temp302; void* _temp304; _LL298: if(( unsigned int) _temp296 > 1u?*((
int*) _temp296) == Cyc_Absyn_Int_c: 0){ _LL305: _temp304=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp296)->f1; goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_Int_c_struct*)
_temp296)->f2; goto _LL299;} else{ goto _LL300;} _LL300: goto _LL301; _LL299: s2=
_temp304; i2= _temp302; goto _LL297; _LL301: Cyc_Evexp_exp_err( e2->loc,({
unsigned char* _temp306=( unsigned char*)"bad constant expression"; struct
_tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 24; _temp307;})); return _temp280; _LL297:;}{
void* _temp308= p; _LL310: if( _temp308 ==( void*) Cyc_Absyn_Div){ goto _LL311;}
else{ goto _LL312;} _LL312: if( _temp308 ==( void*) Cyc_Absyn_Mod){ goto _LL313;}
else{ goto _LL314;} _LL314: goto _LL315; _LL311: goto _LL313; _LL313: if( i2 ==
0){ Cyc_Evexp_exp_err( e2->loc,({ unsigned char* _temp316=( unsigned char*)"division by zero in constant expression";
struct _tagged_string _temp317; _temp317.curr= _temp316; _temp317.base= _temp316;
_temp317.last_plus_one= _temp316 + 40; _temp317;})); return _temp280;} goto
_LL309; _LL315: goto _LL309; _LL309:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp319=({ struct
_tuple5 _temp318; _temp318.f1= p; _temp318.f2= has_u_arg; _temp318;}); int
_temp375; void* _temp377; int _temp379; void* _temp381; int _temp383; void*
_temp385; int _temp387; void* _temp389; int _temp391; void* _temp393; int
_temp395; void* _temp397; int _temp399; void* _temp401; int _temp403; void*
_temp405; int _temp407; void* _temp409; int _temp411; void* _temp413; int
_temp415; void* _temp417; int _temp419; void* _temp421; int _temp423; void*
_temp425; int _temp427; void* _temp429; int _temp431; void* _temp433; int
_temp435; void* _temp437; int _temp439; void* _temp441; int _temp443; void*
_temp445; int _temp447; void* _temp449; int _temp451; void* _temp453; int
_temp455; void* _temp457; int _temp459; void* _temp461; int _temp463; void*
_temp465; int _temp467; void* _temp469; int _temp471; void* _temp473; int
_temp475; void* _temp477; _LL321: _LL378: _temp377= _temp319.f1; if( _temp377 ==(
void*) Cyc_Absyn_Plus){ goto _LL376;} else{ goto _LL323;} _LL376: _temp375=
_temp319.f2; if( _temp375 == 0){ goto _LL322;} else{ goto _LL323;} _LL323:
_LL382: _temp381= _temp319.f1; if( _temp381 ==( void*) Cyc_Absyn_Times){ goto
_LL380;} else{ goto _LL325;} _LL380: _temp379= _temp319.f2; if( _temp379 == 0){
goto _LL324;} else{ goto _LL325;} _LL325: _LL386: _temp385= _temp319.f1; if(
_temp385 ==( void*) Cyc_Absyn_Minus){ goto _LL384;} else{ goto _LL327;} _LL384:
_temp383= _temp319.f2; if( _temp383 == 0){ goto _LL326;} else{ goto _LL327;}
_LL327: _LL390: _temp389= _temp319.f1; if( _temp389 ==( void*) Cyc_Absyn_Div){
goto _LL388;} else{ goto _LL329;} _LL388: _temp387= _temp319.f2; if( _temp387 ==
0){ goto _LL328;} else{ goto _LL329;} _LL329: _LL394: _temp393= _temp319.f1; if(
_temp393 ==( void*) Cyc_Absyn_Mod){ goto _LL392;} else{ goto _LL331;} _LL392:
_temp391= _temp319.f2; if( _temp391 == 0){ goto _LL330;} else{ goto _LL331;}
_LL331: _LL398: _temp397= _temp319.f1; if( _temp397 ==( void*) Cyc_Absyn_Plus){
goto _LL396;} else{ goto _LL333;} _LL396: _temp395= _temp319.f2; if( _temp395 ==
1){ goto _LL332;} else{ goto _LL333;} _LL333: _LL402: _temp401= _temp319.f1; if(
_temp401 ==( void*) Cyc_Absyn_Times){ goto _LL400;} else{ goto _LL335;} _LL400:
_temp399= _temp319.f2; if( _temp399 == 1){ goto _LL334;} else{ goto _LL335;}
_LL335: _LL406: _temp405= _temp319.f1; if( _temp405 ==( void*) Cyc_Absyn_Minus){
goto _LL404;} else{ goto _LL337;} _LL404: _temp403= _temp319.f2; if( _temp403 ==
1){ goto _LL336;} else{ goto _LL337;} _LL337: _LL410: _temp409= _temp319.f1; if(
_temp409 ==( void*) Cyc_Absyn_Div){ goto _LL408;} else{ goto _LL339;} _LL408:
_temp407= _temp319.f2; if( _temp407 == 1){ goto _LL338;} else{ goto _LL339;}
_LL339: _LL414: _temp413= _temp319.f1; if( _temp413 ==( void*) Cyc_Absyn_Mod){
goto _LL412;} else{ goto _LL341;} _LL412: _temp411= _temp319.f2; if( _temp411 ==
1){ goto _LL340;} else{ goto _LL341;} _LL341: _LL418: _temp417= _temp319.f1; if(
_temp417 ==( void*) Cyc_Absyn_Eq){ goto _LL416;} else{ goto _LL343;} _LL416:
_temp415= _temp319.f2; goto _LL342; _LL343: _LL422: _temp421= _temp319.f1; if(
_temp421 ==( void*) Cyc_Absyn_Neq){ goto _LL420;} else{ goto _LL345;} _LL420:
_temp419= _temp319.f2; goto _LL344; _LL345: _LL426: _temp425= _temp319.f1; if(
_temp425 ==( void*) Cyc_Absyn_Gt){ goto _LL424;} else{ goto _LL347;} _LL424:
_temp423= _temp319.f2; if( _temp423 == 0){ goto _LL346;} else{ goto _LL347;}
_LL347: _LL430: _temp429= _temp319.f1; if( _temp429 ==( void*) Cyc_Absyn_Lt){
goto _LL428;} else{ goto _LL349;} _LL428: _temp427= _temp319.f2; if( _temp427 ==
0){ goto _LL348;} else{ goto _LL349;} _LL349: _LL434: _temp433= _temp319.f1; if(
_temp433 ==( void*) Cyc_Absyn_Gte){ goto _LL432;} else{ goto _LL351;} _LL432:
_temp431= _temp319.f2; if( _temp431 == 0){ goto _LL350;} else{ goto _LL351;}
_LL351: _LL438: _temp437= _temp319.f1; if( _temp437 ==( void*) Cyc_Absyn_Lte){
goto _LL436;} else{ goto _LL353;} _LL436: _temp435= _temp319.f2; if( _temp435 ==
0){ goto _LL352;} else{ goto _LL353;} _LL353: _LL442: _temp441= _temp319.f1; if(
_temp441 ==( void*) Cyc_Absyn_Gt){ goto _LL440;} else{ goto _LL355;} _LL440:
_temp439= _temp319.f2; if( _temp439 == 1){ goto _LL354;} else{ goto _LL355;}
_LL355: _LL446: _temp445= _temp319.f1; if( _temp445 ==( void*) Cyc_Absyn_Lt){
goto _LL444;} else{ goto _LL357;} _LL444: _temp443= _temp319.f2; if( _temp443 ==
1){ goto _LL356;} else{ goto _LL357;} _LL357: _LL450: _temp449= _temp319.f1; if(
_temp449 ==( void*) Cyc_Absyn_Gte){ goto _LL448;} else{ goto _LL359;} _LL448:
_temp447= _temp319.f2; if( _temp447 == 1){ goto _LL358;} else{ goto _LL359;}
_LL359: _LL454: _temp453= _temp319.f1; if( _temp453 ==( void*) Cyc_Absyn_Lte){
goto _LL452;} else{ goto _LL361;} _LL452: _temp451= _temp319.f2; if( _temp451 ==
1){ goto _LL360;} else{ goto _LL361;} _LL361: _LL458: _temp457= _temp319.f1; if(
_temp457 ==( void*) Cyc_Absyn_Bitand){ goto _LL456;} else{ goto _LL363;} _LL456:
_temp455= _temp319.f2; goto _LL362; _LL363: _LL462: _temp461= _temp319.f1; if(
_temp461 ==( void*) Cyc_Absyn_Bitor){ goto _LL460;} else{ goto _LL365;} _LL460:
_temp459= _temp319.f2; goto _LL364; _LL365: _LL466: _temp465= _temp319.f1; if(
_temp465 ==( void*) Cyc_Absyn_Bitxor){ goto _LL464;} else{ goto _LL367;} _LL464:
_temp463= _temp319.f2; goto _LL366; _LL367: _LL470: _temp469= _temp319.f1; if(
_temp469 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL468;} else{ goto _LL369;}
_LL468: _temp467= _temp319.f2; goto _LL368; _LL369: _LL474: _temp473= _temp319.f1;
if( _temp473 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL472;} else{ goto _LL371;}
_LL472: _temp471= _temp319.f2; goto _LL370; _LL371: _LL478: _temp477= _temp319.f1;
if( _temp477 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL476;} else{ goto _LL373;}
_LL476: _temp475= _temp319.f2; goto _LL372; _LL373: goto _LL374; _LL322: i3= i1
+ i2; use_i3= 1; goto _LL320; _LL324: i3= i1 * i2; use_i3= 1; goto _LL320;
_LL326: i3= i1 - i2; use_i3= 1; goto _LL320; _LL328: i3= i1 / i2; use_i3= 1;
goto _LL320; _LL330: i3= i1 % i2; use_i3= 1; goto _LL320; _LL332: u3= u1 + u2;
use_u3= 1; goto _LL320; _LL334: u3= u1 * u2; use_u3= 1; goto _LL320; _LL336: u3=
u1 - u2; use_u3= 1; goto _LL320; _LL338: u3= u1 / u2; use_u3= 1; goto _LL320;
_LL340: u3= u1 % u2; use_u3= 1; goto _LL320; _LL342: b3= i1 == i2; use_b3= 1;
goto _LL320; _LL344: b3= i1 != i2; use_b3= 1; goto _LL320; _LL346: b3= i1 > i2;
use_b3= 1; goto _LL320; _LL348: b3= i1 < i2; use_b3= 1; goto _LL320; _LL350: b3=
i1 >= i2; use_b3= 1; goto _LL320; _LL352: b3= i1 <= i2; use_b3= 1; goto _LL320;
_LL354: b3= u1 > u2; use_b3= 1; goto _LL320; _LL356: b3= u1 < u2; use_b3= 1;
goto _LL320; _LL358: b3= u1 >= u2; use_b3= 1; goto _LL320; _LL360: b3= u1 <= u2;
use_b3= 1; goto _LL320; _LL362: u3= u1 & u2; use_u3= 1; goto _LL320; _LL364: u3=
u1 | u2; use_u3= 1; goto _LL320; _LL366: u3= u1 ^ u2; use_u3= 1; goto _LL320;
_LL368: u3= u1 << u2; use_u3= 1; goto _LL320; _LL370: u3= u1 >> u2; use_u3= 1;
goto _LL320; _LL372: Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp479=(
unsigned char*)">>> NOT IMPLEMENTED"; struct _tagged_string _temp480; _temp480.curr=
_temp479; _temp480.base= _temp479; _temp480.last_plus_one= _temp479 + 20;
_temp480;})); return _temp280; _LL374: Cyc_Evexp_exp_err( e1->loc,({
unsigned char* _temp481=( unsigned char*)"bad constant expression"; struct
_tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 24; _temp482;})); return _temp280; _LL320:;}
if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp483=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp483[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp484; _temp484.tag= Cyc_Absyn_Int_c;
_temp484.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp484.f2= i3; _temp484;});
_temp483;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp485=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp485[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp486; _temp486.tag= Cyc_Absyn_Int_c;
_temp486.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp486.f2=( int) u3;
_temp486;}); _temp485;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp487=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp487[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp488; _temp488.tag= Cyc_Absyn_Int_c;
_temp488.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp488.f2= b3? 1: 0; _temp488;});
_temp487;});}( void) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp489=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp489[ 0]=({ struct Cyc_Core_Unreachable_struct _temp490; _temp490.tag= Cyc_Core_Unreachable;
_temp490.f1=({ unsigned char* _temp491=( unsigned char*)"Evexp::eval_const_binop";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 24; _temp492;}); _temp490;}); _temp489;}));}}}}
void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e){ void* cn;{ void*
_temp493=( void*) e->r; void* _temp513; struct Cyc_Absyn_Exp* _temp515; struct
Cyc_Absyn_Exp* _temp517; struct Cyc_Absyn_Exp* _temp519; struct Cyc_List_List*
_temp521; void* _temp523; void* _temp525; struct Cyc_Absyn_Exp* _temp527; struct
_tagged_string* _temp529; void* _temp531; struct Cyc_Absyn_Exp* _temp533; void*
_temp535; struct Cyc_Absyn_Enumfield* _temp537; struct Cyc_Absyn_Enumdecl*
_temp539; struct _tuple0* _temp541; _LL495: if(*(( int*) _temp493) == Cyc_Absyn_Const_e){
_LL514: _temp513=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp493)->f1;
goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp493) == Cyc_Absyn_Conditional_e){
_LL520: _temp519=(( struct Cyc_Absyn_Conditional_e_struct*) _temp493)->f1; goto
_LL518; _LL518: _temp517=(( struct Cyc_Absyn_Conditional_e_struct*) _temp493)->f2;
goto _LL516; _LL516: _temp515=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp493)->f3; goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp493)
== Cyc_Absyn_Primop_e){ _LL524: _temp523=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp493)->f1; goto _LL522; _LL522: _temp521=(( struct Cyc_Absyn_Primop_e_struct*)
_temp493)->f2; goto _LL500;} else{ goto _LL501;} _LL501: if(*(( int*) _temp493)
== Cyc_Absyn_Sizeoftyp_e){ _LL526: _temp525=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp493)->f1; goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp493)
== Cyc_Absyn_Sizeofexp_e){ _LL528: _temp527=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp493)->f1; goto _LL504;} else{ goto _LL505;} _LL505: if(*(( int*) _temp493)
== Cyc_Absyn_Offsetof_e){ _LL532: _temp531=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp493)->f1; goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp493)->f2; goto _LL506;} else{ goto _LL507;} _LL507: if(*(( int*) _temp493)
== Cyc_Absyn_Cast_e){ _LL536: _temp535=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp493)->f1; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_Cast_e_struct*)
_temp493)->f2; goto _LL508;} else{ goto _LL509;} _LL509: if(*(( int*) _temp493)
== Cyc_Absyn_Enum_e){ _LL542: _temp541=(( struct Cyc_Absyn_Enum_e_struct*)
_temp493)->f1; goto _LL540; _LL540: _temp539=(( struct Cyc_Absyn_Enum_e_struct*)
_temp493)->f2; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_Enum_e_struct*)
_temp493)->f3; goto _LL510;} else{ goto _LL511;} _LL511: goto _LL512; _LL496:
return _temp513; _LL498: cn= Cyc_Evexp_eval_const_bool_exp( _temp519)? Cyc_Evexp_eval_const_exp(
_temp517): Cyc_Evexp_eval_const_exp( _temp515); goto _LL494; _LL500: if(
_temp521 == 0){ Cyc_Evexp_exp_err( e->loc,({ unsigned char* _temp543=(
unsigned char*)"bad static expression (no args to primop)"; struct
_tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 42; _temp544;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp545=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp545[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp546; _temp546.tag= Cyc_Absyn_Int_c;
_temp546.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp546.f2= 0; _temp546;});
_temp545;});} if((( struct Cyc_List_List*) _check_null( _temp521))->tl == 0){ cn=
Cyc_Evexp_eval_const_unprimop( _temp523,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp521))->hd);} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp521))->tl))->tl != 0){ Cyc_Evexp_exp_err(
e->loc,({ unsigned char* _temp547=( unsigned char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 48; _temp548;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp549=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp549[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp550; _temp550.tag= Cyc_Absyn_Int_c;
_temp550.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp550.f2= 0; _temp550;});
_temp549;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp523,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp521))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp521))->tl))->hd);}} goto _LL494; _LL502: goto _LL504; _LL504: Cyc_Evexp_exp_err(
e->loc,({ unsigned char* _temp551=( unsigned char*)"sizeof(...) is not an integral constant expression in Cyclone";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 62; _temp552;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp553=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp553[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp554; _temp554.tag= Cyc_Absyn_Int_c;
_temp554.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp554.f2= 0; _temp554;});
_temp553;}); _LL506: Cyc_Evexp_exp_err( e->loc,({ unsigned char* _temp555=(
unsigned char*)"offsetof(...) is not an integral constant expression in Cyclone";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 64; _temp556;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp557=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp557[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp558; _temp558.tag= Cyc_Absyn_Int_c;
_temp558.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp558.f2= 0; _temp558;});
_temp557;}); _LL508: cn= Cyc_Evexp_eval_const_exp( _temp533);{ struct _tuple4
_temp560=({ struct _tuple4 _temp559; _temp559.f1= Cyc_Tcutil_compress( _temp535);
_temp559.f2= cn; _temp559;}); void* _temp570; unsigned char _temp572; void*
_temp574; void* _temp576; void* _temp578; void* _temp580; void* _temp582; short
_temp584; void* _temp586; void* _temp588; void* _temp590; void* _temp592; void*
_temp594; int _temp596; void* _temp598; void* _temp600; void* _temp602; void*
_temp604; _LL562: _LL577: _temp576= _temp560.f1; if(( unsigned int) _temp576 > 4u?*((
int*) _temp576) == Cyc_Absyn_IntType: 0){ _LL581: _temp580=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp576)->f1; goto _LL579; _LL579: _temp578=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp576)->f2; goto _LL571;} else{ goto _LL564;} _LL571: _temp570= _temp560.f2;
if(( unsigned int) _temp570 > 1u?*(( int*) _temp570) == Cyc_Absyn_Char_c: 0){
_LL575: _temp574=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp570)->f1; goto
_LL573; _LL573: _temp572=(( struct Cyc_Absyn_Char_c_struct*) _temp570)->f2; goto
_LL563;} else{ goto _LL564;} _LL564: _LL589: _temp588= _temp560.f1; if((
unsigned int) _temp588 > 4u?*(( int*) _temp588) == Cyc_Absyn_IntType: 0){ _LL593:
_temp592=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp588)->f1; goto _LL591;
_LL591: _temp590=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp588)->f2;
goto _LL583;} else{ goto _LL566;} _LL583: _temp582= _temp560.f2; if((
unsigned int) _temp582 > 1u?*(( int*) _temp582) == Cyc_Absyn_Short_c: 0){ _LL587:
_temp586=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp582)->f1; goto _LL585;
_LL585: _temp584=(( struct Cyc_Absyn_Short_c_struct*) _temp582)->f2; goto _LL565;}
else{ goto _LL566;} _LL566: _LL601: _temp600= _temp560.f1; if(( unsigned int)
_temp600 > 4u?*(( int*) _temp600) == Cyc_Absyn_IntType: 0){ _LL605: _temp604=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp600)->f1; goto _LL603; _LL603:
_temp602=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp600)->f2; goto _LL595;}
else{ goto _LL568;} _LL595: _temp594= _temp560.f2; if(( unsigned int) _temp594 >
1u?*(( int*) _temp594) == Cyc_Absyn_Int_c: 0){ _LL599: _temp598=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp594)->f1; goto _LL597; _LL597: _temp596=((
struct Cyc_Absyn_Int_c_struct*) _temp594)->f2; goto _LL567;} else{ goto _LL568;}
_LL568: goto _LL569; _LL563: _temp592= _temp580; _temp590= _temp578; _temp586=
_temp574; _temp584=( short) _temp572; goto _LL565; _LL565: _temp604= _temp592;
_temp602= _temp590; _temp598= _temp586; _temp596=( int) _temp584; goto _LL567;
_LL567: if( _temp604 != _temp598){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp606=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp606[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp607; _temp607.tag= Cyc_Absyn_Int_c;
_temp607.f1=( void*) _temp604; _temp607.f2= _temp596; _temp607;}); _temp606;});}
goto _LL561; _LL569: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ struct
_tagged_string _temp608= Cyc_Absynpp_typ2string( _temp535); xprintf("eval_const: cannot cast to %.*s",
_temp608.last_plus_one - _temp608.curr, _temp608.curr);})); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp609=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp609[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp610; _temp610.tag= Cyc_Absyn_Int_c; _temp610.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp610.f2= 0; _temp610;}); _temp609;}); _LL561:;} goto _LL494; _LL510: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_check_null( _temp537))->tag)); _LL512: Cyc_Evexp_exp_err( e->loc,({
unsigned char* _temp611=( unsigned char*)"bad static expression"; struct
_tagged_string _temp612; _temp612.curr= _temp611; _temp612.base= _temp611;
_temp612.last_plus_one= _temp611 + 22; _temp612;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp613=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp613[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp614; _temp614.tag= Cyc_Absyn_Int_c;
_temp614.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp614.f2= 0; _temp614;});
_temp613;}); _LL494:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp615=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp615[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp616; _temp616.tag= Cyc_Absyn_Const_e;
_temp616.f1=( void*) cn; _temp616;}); _temp615;}))); return cn;}