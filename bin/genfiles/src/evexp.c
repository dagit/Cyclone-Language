 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern struct Cyc_Position_Error*
Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*, struct _tagged_string);
extern unsigned char Cyc_Position_Nocontext[ 14u]; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
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
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
unsigned int Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment* loc); void
Cyc_Evexp_exp_err( struct Cyc_Position_Segment* loc, struct _tagged_string msg){
Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,({ struct _tagged_string
_temp0= msg; xprintf("Exp_err: %.*s", _temp0.last_plus_one - _temp0.curr, _temp0.curr);})));}
extern void* Cyc_Evexp_promote_const( void* cn); extern void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_Evexp_eval_const_unprimop( void* p,
struct Cyc_Absyn_Exp* e); extern void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2); extern void* Cyc_Evexp_eval_sizeof(
void* t, struct Cyc_Position_Segment* loc); unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp7; void* _temp9; _LL3: if(( unsigned int) _temp1 > 1u?*(( int*)
_temp1) == Cyc_Absyn_Int_c: 0){ _LL10: _temp9=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1)->f1; goto _LL8; _LL8: _temp7=(( struct Cyc_Absyn_Int_c_struct*) _temp1)->f2;
goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: return( unsigned int)
_temp7; _LL6: Cyc_Evexp_exp_err( e->loc,({ unsigned char* _temp11="expecting unsigned int";
struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base= _temp11;
_temp12.last_plus_one= _temp11 + 23; _temp12;})); return 0u; _LL2:;} void* Cyc_Evexp_promote_const(
void* cn){ void* _temp13= cn; unsigned char _temp21; void* _temp23; short
_temp25; void* _temp27; _LL15: if(( unsigned int) _temp13 > 1u?*(( int*) _temp13)
== Cyc_Absyn_Char_c: 0){ _LL24: _temp23=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f1; goto _LL22; _LL22: _temp21=(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f2; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp13
> 1u?*(( int*) _temp13) == Cyc_Absyn_Short_c: 0){ _LL28: _temp27=( void*)((
struct Cyc_Absyn_Short_c_struct*) _temp13)->f1; goto _LL26; _LL26: _temp25=((
struct Cyc_Absyn_Short_c_struct*) _temp13)->f2; goto _LL18;} else{ goto _LL19;}
_LL19: goto _LL20; _LL16: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp29=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp30; _temp30.tag= Cyc_Absyn_Int_c;
_temp30.f1=( void*) _temp23; _temp30.f2=( int) _temp21; _temp30;}); _temp29;});
_LL18: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp31=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp31[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp32; _temp32.tag= Cyc_Absyn_Int_c; _temp32.f1=( void*) _temp27; _temp32.f2=(
int) _temp25; _temp32;}); _temp31;}); _LL20: return cn; _LL14:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp33= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
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
unsigned char* _temp51="expecting bool"; struct _tagged_string _temp52; _temp52.curr=
_temp51; _temp52.base= _temp51; _temp52.last_plus_one= _temp51 + 15; _temp52;}));
return 0; _LL34:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e){ void* cn;{ void* _temp53=( void*) e->r; void* _temp71;
struct Cyc_Absyn_Exp* _temp73; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp*
_temp77; struct Cyc_List_List* _temp79; void* _temp81; void* _temp83; struct Cyc_Absyn_Exp*
_temp85; struct Cyc_Absyn_Exp* _temp87; void* _temp89; struct Cyc_Absyn_Enumfield*
_temp91; struct Cyc_Absyn_Enumdecl* _temp93; struct _tuple0* _temp95; _LL55: if(*((
int*) _temp53) == Cyc_Absyn_Const_e){ _LL72: _temp71=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp53)->f1; goto _LL56;} else{ goto _LL57;} _LL57: if(*(( int*) _temp53) ==
Cyc_Absyn_Conditional_e){ _LL78: _temp77=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f1; goto _LL76; _LL76: _temp75=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f2; goto _LL74; _LL74: _temp73=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f3; goto _LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp53) ==
Cyc_Absyn_Primop_e){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f1; goto _LL80; _LL80: _temp79=(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f2; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp53) ==
Cyc_Absyn_Sizeoftyp_e){ _LL84: _temp83=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp53)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp53) ==
Cyc_Absyn_Sizeofexp_e){ _LL86: _temp85=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp53)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if(*(( int*) _temp53) ==
Cyc_Absyn_Cast_e){ _LL90: _temp89=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f1; goto _LL88; _LL88: _temp87=(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f2; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp53) ==
Cyc_Absyn_Enum_e){ _LL96: _temp95=(( struct Cyc_Absyn_Enum_e_struct*) _temp53)->f1;
goto _LL94; _LL94: _temp93=(( struct Cyc_Absyn_Enum_e_struct*) _temp53)->f2;
goto _LL92; _LL92: _temp91=(( struct Cyc_Absyn_Enum_e_struct*) _temp53)->f3;
goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL56: return _temp71; _LL58:
cn= Cyc_Evexp_eval_const_bool_exp( _temp77)? Cyc_Evexp_eval_const_exp( _temp75):
Cyc_Evexp_eval_const_exp( _temp73); goto _LL54; _LL60: if( _temp79 == 0){ Cyc_Evexp_exp_err(
e->loc,({ unsigned char* _temp97="bad static expression (no args to primop)";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 42; _temp98;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp99=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp99[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp100; _temp100.tag= Cyc_Absyn_Int_c;
_temp100.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp100.f2= 0; _temp100;});
_temp99;});} if((( struct Cyc_List_List*) _check_null( _temp79))->tl == 0){ cn=
Cyc_Evexp_eval_const_unprimop( _temp81,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp79))->hd);} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp79))->tl))->tl != 0){ Cyc_Evexp_exp_err(
e->loc,({ unsigned char* _temp101="bad static expression (too many args to primop)";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 48; _temp102;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp103=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp104; _temp104.tag= Cyc_Absyn_Int_c;
_temp104.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp104.f2= 0; _temp104;});
_temp103;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp81,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp79))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp79))->tl))->hd);}} goto _LL54; _LL62: cn= Cyc_Evexp_eval_sizeof( _temp83, e->loc);
goto _LL54; _LL64: cn= Cyc_Evexp_eval_sizeof(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp85->topt))->v, _temp85->loc); goto _LL54; _LL66: cn= Cyc_Evexp_eval_const_exp(
_temp87);{ struct _tuple3 _temp106=({ struct _tuple3 _temp105; _temp105.f1= Cyc_Tcutil_compress(
_temp89); _temp105.f2= cn; _temp105;}); void* _temp116; unsigned char _temp118;
void* _temp120; void* _temp122; void* _temp124; void* _temp126; void* _temp128;
short _temp130; void* _temp132; void* _temp134; void* _temp136; void* _temp138;
void* _temp140; int _temp142; void* _temp144; void* _temp146; void* _temp148;
void* _temp150; _LL108: _LL123: _temp122= _temp106.f1; if(( unsigned int)
_temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_IntType: 0){ _LL127: _temp126=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp122)->f1; goto _LL125; _LL125:
_temp124=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp122)->f2; goto _LL117;}
else{ goto _LL110;} _LL117: _temp116= _temp106.f2; if(( unsigned int) _temp116 >
1u?*(( int*) _temp116) == Cyc_Absyn_Char_c: 0){ _LL121: _temp120=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp116)->f1; goto _LL119; _LL119: _temp118=((
struct Cyc_Absyn_Char_c_struct*) _temp116)->f2; goto _LL109;} else{ goto _LL110;}
_LL110: _LL135: _temp134= _temp106.f1; if(( unsigned int) _temp134 > 4u?*(( int*)
_temp134) == Cyc_Absyn_IntType: 0){ _LL139: _temp138=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp134)->f1; goto _LL137; _LL137: _temp136=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp134)->f2; goto _LL129;} else{ goto _LL112;} _LL129: _temp128= _temp106.f2;
if(( unsigned int) _temp128 > 1u?*(( int*) _temp128) == Cyc_Absyn_Short_c: 0){
_LL133: _temp132=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp128)->f1;
goto _LL131; _LL131: _temp130=(( struct Cyc_Absyn_Short_c_struct*) _temp128)->f2;
goto _LL111;} else{ goto _LL112;} _LL112: _LL147: _temp146= _temp106.f1; if((
unsigned int) _temp146 > 4u?*(( int*) _temp146) == Cyc_Absyn_IntType: 0){ _LL151:
_temp150=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp146)->f1; goto _LL149;
_LL149: _temp148=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp146)->f2;
goto _LL141;} else{ goto _LL114;} _LL141: _temp140= _temp106.f2; if((
unsigned int) _temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Int_c: 0){ _LL145:
_temp144=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp140)->f1; goto _LL143;
_LL143: _temp142=(( struct Cyc_Absyn_Int_c_struct*) _temp140)->f2; goto _LL113;}
else{ goto _LL114;} _LL114: goto _LL115; _LL109: _temp138= _temp126; _temp136=
_temp124; _temp132= _temp120; _temp130=( short) _temp118; goto _LL111; _LL111:
_temp150= _temp138; _temp148= _temp136; _temp144= _temp132; _temp142=( int)
_temp130; goto _LL113; _LL113: if( _temp150 != _temp144){ cn=( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp152=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp152[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp153; _temp153.tag= Cyc_Absyn_Int_c; _temp153.f1=( void*) _temp150; _temp153.f2=
_temp142; _temp153;}); _temp152;});} goto _LL107; _LL115: Cyc_Evexp_exp_err( e->loc,({
struct _tagged_string _temp154= Cyc_Absynpp_typ2string( _temp89); xprintf("eval_const: cannot cast to %.*s",
_temp154.last_plus_one - _temp154.curr, _temp154.curr);})); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp155=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp155[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp156; _temp156.tag= Cyc_Absyn_Int_c; _temp156.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp156.f2= 0; _temp156;}); _temp155;}); _LL107:;} goto _LL54; _LL68: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_check_null( _temp91))->tag)); _LL70: Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp157="bad static expression"; struct _tagged_string _temp158; _temp158.curr=
_temp157; _temp158.base= _temp157; _temp158.last_plus_one= _temp157 + 22;
_temp158;})); return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp159=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp159[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp160; _temp160.tag= Cyc_Absyn_Int_c;
_temp160.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp160.f2= 0; _temp160;});
_temp159;}); _LL54:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp161=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp161[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp162; _temp162.tag= Cyc_Absyn_Const_e;
_temp162.f1=( void*) cn; _temp162;}); _temp161;}))); return cn;} struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; } ; unsigned int Cyc_Evexp_szof( void* t,
struct Cyc_Position_Segment* loc){ void* _temp163= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp219; int _temp221; struct Cyc_Core_Opt* _temp223;
void* _temp225; int _temp227; struct Cyc_Core_Opt* _temp229; void* _temp231;
struct Cyc_Absyn_TunionInfo _temp233; struct Cyc_Absyn_TunionFieldInfo _temp235;
struct Cyc_List_List* _temp237; void* _temp239; struct Cyc_Absyn_Tunionfield*
_temp241; struct Cyc_Absyn_Tuniondecl* _temp243; struct Cyc_Absyn_TunionFieldInfo
_temp245; struct Cyc_Absyn_PtrInfo _temp247; struct Cyc_Absyn_Conref* _temp249;
struct Cyc_Absyn_Tqual _temp251; struct Cyc_Absyn_Conref* _temp253; void*
_temp255; void* _temp257; void* _temp259; void* _temp261; void* _temp263; void*
_temp265; void* _temp267; void* _temp269; void* _temp271; void* _temp273; struct
Cyc_Absyn_Exp* _temp275; struct Cyc_Absyn_Tqual _temp277; void* _temp279; struct
Cyc_Absyn_FnInfo _temp281; struct Cyc_List_List* _temp283; void* _temp285;
struct Cyc_Absyn_Structdecl** _temp287; struct Cyc_List_List* _temp289; struct
_tuple0* _temp291; struct Cyc_List_List* _temp293; struct Cyc_Absyn_Uniondecl**
_temp295; struct Cyc_List_List* _temp297; struct _tuple0* _temp299; struct Cyc_List_List*
_temp301; struct Cyc_Absyn_Enumdecl* _temp303; struct _tuple0* _temp305; struct
Cyc_Core_Opt* _temp307; struct Cyc_List_List* _temp309; struct _tuple0* _temp311;
struct Cyc_List_List* _temp313; void* _temp315; _LL165: if( _temp163 ==( void*)
Cyc_Absyn_VoidType){ goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_VarType: 0){ _LL220: _temp219=((
struct Cyc_Absyn_VarType_struct*) _temp163)->f1; goto _LL168;} else{ goto _LL169;}
_LL169: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_Evar:
0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp163)->f1;
if( _temp225 ==( void*) Cyc_Absyn_BoxKind){ goto _LL224;} else{ goto _LL171;}
_LL224: _temp223=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f2; goto _LL222;
_LL222: _temp221=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f3; goto _LL170;}
else{ goto _LL171;} _LL171: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_Evar: 0){ _LL232: _temp231=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp163)->f1; goto _LL230; _LL230: _temp229=(( struct Cyc_Absyn_Evar_struct*)
_temp163)->f2; goto _LL228; _LL228: _temp227=(( struct Cyc_Absyn_Evar_struct*)
_temp163)->f3; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_TunionType: 0){ _LL234: _temp233=((
struct Cyc_Absyn_TunionType_struct*) _temp163)->f1; goto _LL174;} else{ goto
_LL175;} _LL175: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_TunionFieldType:
0){ _LL236: _temp235=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp163)->f1;
_LL240: _temp239=( void*) _temp235.field_info; if(*(( int*) _temp239) == Cyc_Absyn_KnownTunionfield){
_LL244: _temp243=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp239)->f1;
goto _LL242; _LL242: _temp241=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp239)->f2; goto _LL238;} else{ goto _LL177;} _LL238: _temp237= _temp235.targs;
goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp163 > 4u?*((
int*) _temp163) == Cyc_Absyn_TunionFieldType: 0){ _LL246: _temp245=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp163)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_PointerType: 0){ _LL248: _temp247=((
struct Cyc_Absyn_PointerType_struct*) _temp163)->f1; _LL258: _temp257=( void*)
_temp247.elt_typ; goto _LL256; _LL256: _temp255=( void*) _temp247.rgn_typ; goto
_LL254; _LL254: _temp253= _temp247.nullable; goto _LL252; _LL252: _temp251=
_temp247.tq; goto _LL250; _LL250: _temp249= _temp247.bounds; goto _LL180;} else{
goto _LL181;} _LL181: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) ==
Cyc_Absyn_IntType: 0){ _LL262: _temp261=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL260; _LL260: _temp259=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp259 ==( void*) Cyc_Absyn_B1){ goto _LL182;} else{ goto
_LL183;}} else{ goto _LL183;} _LL183: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL266: _temp265=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL264; _LL264: _temp263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp263 ==( void*) Cyc_Absyn_B2){ goto _LL184;} else{ goto
_LL185;}} else{ goto _LL185;} _LL185: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL270: _temp269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL268; _LL268: _temp267=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp267 ==( void*) Cyc_Absyn_B4){ goto _LL186;} else{ goto
_LL187;}} else{ goto _LL187;} _LL187: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL274: _temp273=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL272; _LL272: _temp271=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp271 ==( void*) Cyc_Absyn_B8){ goto _LL188;} else{ goto
_LL189;}} else{ goto _LL189;} _LL189: if( _temp163 ==( void*) Cyc_Absyn_FloatType){
goto _LL190;} else{ goto _LL191;} _LL191: if( _temp163 ==( void*) Cyc_Absyn_DoubleType){
goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int) _temp163 > 4u?*((
int*) _temp163) == Cyc_Absyn_ArrayType: 0){ _LL280: _temp279=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp163)->f1; goto _LL278; _LL278: _temp277=((
struct Cyc_Absyn_ArrayType_struct*) _temp163)->f2; goto _LL276; _LL276: _temp275=((
struct Cyc_Absyn_ArrayType_struct*) _temp163)->f3; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_FnType:
0){ _LL282: _temp281=(( struct Cyc_Absyn_FnType_struct*) _temp163)->f1; goto
_LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_TupleType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_TupleType_struct*)
_temp163)->f1; goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_RgnHandleType: 0){ _LL286:
_temp285=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp163)->f1; goto
_LL200;} else{ goto _LL201;} _LL201: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_StructType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f1; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f2; goto _LL288; _LL288: _temp287=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f3; goto _LL202;} else{ goto _LL203;} _LL203: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AnonStructType: 0){ _LL294:
_temp293=(( struct Cyc_Absyn_AnonStructType_struct*) _temp163)->f1; goto _LL204;}
else{ goto _LL205;} _LL205: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_UnionType: 0){ _LL300: _temp299=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f1; goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f2; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f3; goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AnonUnionType: 0){ _LL302:
_temp301=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp163)->f1; goto _LL208;}
else{ goto _LL209;} _LL209: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_EnumType: 0){ _LL306: _temp305=(( struct Cyc_Absyn_EnumType_struct*)
_temp163)->f1; goto _LL304; _LL304: _temp303=(( struct Cyc_Absyn_EnumType_struct*)
_temp163)->f2; goto _LL210;} else{ goto _LL211;} _LL211: if( _temp163 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_TypedefType: 0){ _LL312: _temp311=((
struct Cyc_Absyn_TypedefType_struct*) _temp163)->f1; goto _LL310; _LL310:
_temp309=(( struct Cyc_Absyn_TypedefType_struct*) _temp163)->f2; goto _LL308;
_LL308: _temp307=(( struct Cyc_Absyn_TypedefType_struct*) _temp163)->f3; goto
_LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_JoinEff: 0){ _LL314: _temp313=(( struct Cyc_Absyn_JoinEff_struct*)
_temp163)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AccessEff: 0){ _LL316: _temp315=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp163)->f1; goto _LL218;} else{
goto _LL164;} _LL166: Cyc_Tcutil_terr( loc,({ unsigned char* _temp317="cannot apply sizeof to void type";
struct _tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 33; _temp318;})); return 0u; _LL168: { void*
_temp319=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp219->kind); _LL321: if( _temp319 ==( void*) Cyc_Absyn_BoxKind){ goto _LL322;}
else{ goto _LL323;} _LL323: if( _temp319 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL324;} else{ goto _LL325;} _LL325: if( _temp319 ==( void*) Cyc_Absyn_MemKind){
goto _LL326;} else{ goto _LL327;} _LL327: if( _temp319 ==( void*) Cyc_Absyn_RgnKind){
goto _LL328;} else{ goto _LL329;} _LL329: if( _temp319 ==( void*) Cyc_Absyn_EffKind){
goto _LL330;} else{ goto _LL320;} _LL322: return 4u; _LL324: goto _LL326; _LL326:
Cyc_Tcutil_terr( loc,({ unsigned char* _temp331="cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 49; _temp332;})); return 0u; _LL328: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp333="cannot apply sizeof to a region"; struct
_tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 32; _temp334;})); return 0u; _LL330: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp335="cannot apply sizeof to an effect"; struct
_tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 33; _temp336;})); return 0u; _LL320:;} _LL170:
return 4u; _LL172: Cyc_Tcutil_terr( loc,({ unsigned char* _temp337="cannot apply sizeof to unknown type";
struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 36; _temp338;})); return 0u; _LL174: return 4u;
_LL176: return 4u + Cyc_Evexp_szof(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp339=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp339[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp340; _temp340.tag= Cyc_Absyn_TupleType;
_temp340.f1= _temp241->typs; _temp340;}); _temp339;}), loc); _LL178:( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char*
_temp341="unroslved tunionfield type in szof"; struct _tagged_string _temp342;
_temp342.curr= _temp341; _temp342.base= _temp341; _temp342.last_plus_one=
_temp341 + 35; _temp342;}))); _LL180: { void* _temp343=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp249))->v; void*
_temp351; void* _temp353; _LL345: if(( unsigned int) _temp343 > 1u?*(( int*)
_temp343) == Cyc_Absyn_Eq_constr: 0){ _LL352: _temp351=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp343)->f1; if( _temp351 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL346;} else{
goto _LL347;}} else{ goto _LL347;} _LL347: if(( unsigned int) _temp343 > 1u?*((
int*) _temp343) == Cyc_Absyn_Eq_constr: 0){ _LL354: _temp353=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp343)->f1; goto _LL348;} else{ goto _LL349;}
_LL349: goto _LL350; _LL346: return 12u; _LL348: return 4u; _LL350: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp355="bounds information not known for pointer type";
struct _tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 46; _temp356;})); return 0u; _LL344:;} _LL182:
return 1u; _LL184: return 1u; _LL186: return 1u; _LL188: return 1u; _LL190:
return 4u; _LL192: return 8u; _LL194: if( _temp275 == 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp357="cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 48; _temp358;}));} return Cyc_Evexp_szof(
_temp279, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp275)); _LL196: Cyc_Tcutil_terr( loc,({ unsigned char* _temp359="cannot apply sizeof to function type";
struct _tagged_string _temp360; _temp360.curr= _temp359; _temp360.base= _temp359;
_temp360.last_plus_one= _temp359 + 37; _temp360;})); return 0u; _LL198: {
unsigned int s= 0; for( 0; _temp283 != 0; _temp283=(( struct Cyc_List_List*)
_check_null( _temp283))->tl){ s += Cyc_Evexp_szof((*(( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( _temp283))->hd)).f2, loc);} return s;} _LL200:
return 4u; _LL202: if( _temp287 == 0){ return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp361="szof on unchecked StructType";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 29; _temp362;}));}{ struct Cyc_Absyn_Structdecl*
_temp363=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp287)); goto _LL364;
_LL364: if( _temp363->fields == 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp365="cannot apply sizeof to abstract struct type"; struct _tagged_string
_temp366; _temp366.curr= _temp365; _temp366.base= _temp365; _temp366.last_plus_one=
_temp365 + 44; _temp366;}));} _temp293=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp363->fields))->v; goto _LL204;} _LL204: { unsigned int s= 0;
for( 0; _temp293 != 0; _temp293=(( struct Cyc_List_List*) _check_null( _temp293))->tl){
s += Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp293))->hd)->type, loc);} return s;} _LL206: if( _temp295 == 0){
return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp367="szof on unchecked UnionType"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 28; _temp368;}));}{ struct Cyc_Absyn_Uniondecl* _temp369=*(( struct
Cyc_Absyn_Uniondecl**) _check_null( _temp295)); goto _LL370; _LL370: if(
_temp369->fields == 0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp371="cannot apply sizeof to abstract union type";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 43; _temp372;}));} _temp301=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp369->fields))->v; goto _LL208;} _LL208: {
unsigned int s= 0; for( 0; _temp301 != 0; _temp301=(( struct Cyc_List_List*)
_check_null( _temp301))->tl){ unsigned int i= Cyc_Evexp_szof(( void*)(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp301))->hd)->type,
loc); s= s > i? s: i;} return s;} _LL210: return 4u; _LL212: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp373="cannot apply sizoef to a region"; struct
_tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 32; _temp374;})); return 0u; _LL214: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp375= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp375.last_plus_one - _temp375.curr, _temp375.curr);})); _LL216: goto _LL218;
_LL218: Cyc_Tcutil_terr( loc,({ unsigned char* _temp376="cannot apply sizoef to an effect";
struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 33; _temp377;})); return 0u; _LL164:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp378=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp378[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp379; _temp379.tag= Cyc_Absyn_Int_c; _temp379.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp379.f2=( int) Cyc_Evexp_szof( t, loc); _temp379;}); _temp378;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp380= cn; unsigned char _temp390; void* _temp392; int
_temp394; void* _temp396; struct _tagged_string _temp398; _LL382: if((
unsigned int) _temp380 > 1u?*(( int*) _temp380) == Cyc_Absyn_Char_c: 0){ _LL393:
_temp392=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp380)->f1; goto _LL391;
_LL391: _temp390=(( struct Cyc_Absyn_Char_c_struct*) _temp380)->f2; goto _LL383;}
else{ goto _LL384;} _LL384: if(( unsigned int) _temp380 > 1u?*(( int*) _temp380)
== Cyc_Absyn_Int_c: 0){ _LL397: _temp396=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp380)->f1; goto _LL395; _LL395: _temp394=(( struct Cyc_Absyn_Int_c_struct*)
_temp380)->f2; goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int)
_temp380 > 1u?*(( int*) _temp380) == Cyc_Absyn_Float_c: 0){ _LL399: _temp398=((
struct Cyc_Absyn_Float_c_struct*) _temp380)->f1; goto _LL387;} else{ goto _LL388;}
_LL388: goto _LL389; _LL383: return 1; _LL385: return 1; _LL387: return 1;
_LL389: return 0; _LL381:;} void* Cyc_Evexp_eval_const_unprimop( void* p, struct
Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp401=({ struct _tuple3 _temp400; _temp400.f1= p;
_temp400.f2= cn; _temp400;}); void* _temp419; void* _temp421; void* _temp423;
int _temp425; void* _temp427; void* _temp429; void* _temp431; short _temp433;
void* _temp435; void* _temp437; void* _temp439; unsigned char _temp441; void*
_temp443; void* _temp445; void* _temp447; int _temp449; void* _temp451; void*
_temp453; void* _temp455; void* _temp457; void* _temp459; int _temp461; void*
_temp463; void* _temp465; _LL403: _LL422: _temp421= _temp401.f1; if( _temp421 ==(
void*) Cyc_Absyn_Plus){ goto _LL420;} else{ goto _LL405;} _LL420: _temp419=
_temp401.f2; goto _LL404; _LL405: _LL430: _temp429= _temp401.f1; if( _temp429 ==(
void*) Cyc_Absyn_Minus){ goto _LL424;} else{ goto _LL407;} _LL424: _temp423=
_temp401.f2; if(( unsigned int) _temp423 > 1u?*(( int*) _temp423) == Cyc_Absyn_Int_c:
0){ _LL428: _temp427=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp423)->f1;
goto _LL426; _LL426: _temp425=(( struct Cyc_Absyn_Int_c_struct*) _temp423)->f2;
goto _LL406;} else{ goto _LL407;} _LL407: _LL438: _temp437= _temp401.f1; if(
_temp437 ==( void*) Cyc_Absyn_Minus){ goto _LL432;} else{ goto _LL409;} _LL432:
_temp431= _temp401.f2; if(( unsigned int) _temp431 > 1u?*(( int*) _temp431) ==
Cyc_Absyn_Short_c: 0){ _LL436: _temp435=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp431)->f1; goto _LL434; _LL434: _temp433=(( struct Cyc_Absyn_Short_c_struct*)
_temp431)->f2; goto _LL408;} else{ goto _LL409;} _LL409: _LL446: _temp445=
_temp401.f1; if( _temp445 ==( void*) Cyc_Absyn_Minus){ goto _LL440;} else{ goto
_LL411;} _LL440: _temp439= _temp401.f2; if(( unsigned int) _temp439 > 1u?*(( int*)
_temp439) == Cyc_Absyn_Char_c: 0){ _LL444: _temp443=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp439)->f1; goto _LL442; _LL442: _temp441=(( struct Cyc_Absyn_Char_c_struct*)
_temp439)->f2; goto _LL410;} else{ goto _LL411;} _LL411: _LL454: _temp453=
_temp401.f1; if( _temp453 ==( void*) Cyc_Absyn_Not){ goto _LL448;} else{ goto
_LL413;} _LL448: _temp447= _temp401.f2; if(( unsigned int) _temp447 > 1u?*(( int*)
_temp447) == Cyc_Absyn_Int_c: 0){ _LL452: _temp451=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp447)->f1; goto _LL450; _LL450: _temp449=(( struct Cyc_Absyn_Int_c_struct*)
_temp447)->f2; goto _LL412;} else{ goto _LL413;} _LL413: _LL458: _temp457=
_temp401.f1; if( _temp457 ==( void*) Cyc_Absyn_Not){ goto _LL456;} else{ goto
_LL415;} _LL456: _temp455= _temp401.f2; if( _temp455 ==( void*) Cyc_Absyn_Null_c){
goto _LL414;} else{ goto _LL415;} _LL415: _LL466: _temp465= _temp401.f1; if(
_temp465 ==( void*) Cyc_Absyn_Bitnot){ goto _LL460;} else{ goto _LL417;} _LL460:
_temp459= _temp401.f2; if(( unsigned int) _temp459 > 1u?*(( int*) _temp459) ==
Cyc_Absyn_Int_c: 0){ _LL464: _temp463=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp459)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_Int_c_struct*)
_temp459)->f2; goto _LL416;} else{ goto _LL417;} _LL417: goto _LL418; _LL404:
if( ! Cyc_Evexp_is_arith_const( cn)){ Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp467="expecting arithmetic constant"; struct _tagged_string _temp468;
_temp468.curr= _temp467; _temp468.base= _temp467; _temp468.last_plus_one=
_temp467 + 30; _temp468;}));} return cn; _LL406: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp469=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp469[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp470; _temp470.tag= Cyc_Absyn_Int_c;
_temp470.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp470.f2= - _temp425;
_temp470;}); _temp469;}); _LL408: return( void*)({ struct Cyc_Absyn_Short_c_struct*
_temp471=( struct Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp471[ 0]=({ struct Cyc_Absyn_Short_c_struct _temp472; _temp472.tag= Cyc_Absyn_Short_c;
_temp472.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp472.f2= - _temp433;
_temp472;}); _temp471;}); _LL410: return( void*)({ struct Cyc_Absyn_Char_c_struct*
_temp473=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp473[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp474; _temp474.tag= Cyc_Absyn_Char_c;
_temp474.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp474.f2= - _temp441;
_temp474;}); _temp473;}); _LL412: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp475=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp475[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp476; _temp476.tag= Cyc_Absyn_Int_c;
_temp476.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp476.f2= _temp449 == 0? 1:
0; _temp476;}); _temp475;}); _LL414: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp477=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp477[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp478; _temp478.tag= Cyc_Absyn_Int_c;
_temp478.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp478.f2= 1; _temp478;});
_temp477;}); _LL416: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp479=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp479[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp480; _temp480.tag= Cyc_Absyn_Int_c;
_temp480.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp480.f2= ~ _temp461;
_temp480;}); _temp479;}); _LL418: Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp481="bad constant expression"; struct _tagged_string _temp482; _temp482.curr=
_temp481; _temp482.base= _temp481; _temp482.last_plus_one= _temp481 + 24;
_temp482;})); return cn; _LL402:;} struct _tuple5{ void* f1; int f2; } ; void*
Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ void* _temp483= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1));
goto _LL484; _LL484: { void* _temp485= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); goto _LL486; _LL486: { void* s1; void* s2; int i1; int i2;{ void* _temp487=
_temp483; int _temp493; void* _temp495; _LL489: if(( unsigned int) _temp487 > 1u?*((
int*) _temp487) == Cyc_Absyn_Int_c: 0){ _LL496: _temp495=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp487)->f1; goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_Int_c_struct*)
_temp487)->f2; goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL490: s1=
_temp495; i1= _temp493; goto _LL488; _LL492: Cyc_Evexp_exp_err( e1->loc,({
unsigned char* _temp497="bad constant expression"; struct _tagged_string
_temp498; _temp498.curr= _temp497; _temp498.base= _temp497; _temp498.last_plus_one=
_temp497 + 24; _temp498;})); return _temp483; _LL488:;}{ void* _temp499=
_temp485; int _temp505; void* _temp507; _LL501: if(( unsigned int) _temp499 > 1u?*((
int*) _temp499) == Cyc_Absyn_Int_c: 0){ _LL508: _temp507=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp499)->f1; goto _LL506; _LL506: _temp505=(( struct Cyc_Absyn_Int_c_struct*)
_temp499)->f2; goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL502: s2=
_temp507; i2= _temp505; goto _LL500; _LL504: Cyc_Evexp_exp_err( e2->loc,({
unsigned char* _temp509="bad constant expression"; struct _tagged_string
_temp510; _temp510.curr= _temp509; _temp510.base= _temp509; _temp510.last_plus_one=
_temp509 + 24; _temp510;})); return _temp483; _LL500:;}{ void* _temp511= p;
_LL513: if( _temp511 ==( void*) Cyc_Absyn_Div){ goto _LL514;} else{ goto _LL515;}
_LL515: if( _temp511 ==( void*) Cyc_Absyn_Mod){ goto _LL516;} else{ goto _LL517;}
_LL517: goto _LL518; _LL514: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,({
unsigned char* _temp519="division by zero in constant expression"; struct
_tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 40; _temp520;})); return _temp483;} goto
_LL512; _LL516: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,({ unsigned char*
_temp521="division by zero in constant expression"; struct _tagged_string
_temp522; _temp522.curr= _temp521; _temp522.base= _temp521; _temp522.last_plus_one=
_temp521 + 40; _temp522;})); return _temp483;} goto _LL512; _LL518: goto _LL512;
_LL512:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned? 1: s2 ==( void*) Cyc_Absyn_Unsigned;
unsigned int u1=( unsigned int) i1; unsigned int u2=( unsigned int) i2; int i3=
0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int use_u3= 0; int use_b3= 0;{
struct _tuple5 _temp524=({ struct _tuple5 _temp523; _temp523.f1= p; _temp523.f2=
has_u_arg; _temp523;}); int _temp580; void* _temp582; int _temp584; void*
_temp586; int _temp588; void* _temp590; int _temp592; void* _temp594; int
_temp596; void* _temp598; int _temp600; void* _temp602; int _temp604; void*
_temp606; int _temp608; void* _temp610; int _temp612; void* _temp614; int
_temp616; void* _temp618; int _temp620; void* _temp622; int _temp624; void*
_temp626; int _temp628; void* _temp630; int _temp632; void* _temp634; int
_temp636; void* _temp638; int _temp640; void* _temp642; int _temp644; void*
_temp646; int _temp648; void* _temp650; int _temp652; void* _temp654; int
_temp656; void* _temp658; int _temp660; void* _temp662; int _temp664; void*
_temp666; int _temp668; void* _temp670; int _temp672; void* _temp674; int
_temp676; void* _temp678; int _temp680; void* _temp682; _LL526: _LL583: _temp582=
_temp524.f1; if( _temp582 ==( void*) Cyc_Absyn_Plus){ goto _LL581;} else{ goto
_LL528;} _LL581: _temp580= _temp524.f2; if( _temp580 == 0){ goto _LL527;} else{
goto _LL528;} _LL528: _LL587: _temp586= _temp524.f1; if( _temp586 ==( void*) Cyc_Absyn_Times){
goto _LL585;} else{ goto _LL530;} _LL585: _temp584= _temp524.f2; if( _temp584 ==
0){ goto _LL529;} else{ goto _LL530;} _LL530: _LL591: _temp590= _temp524.f1; if(
_temp590 ==( void*) Cyc_Absyn_Minus){ goto _LL589;} else{ goto _LL532;} _LL589:
_temp588= _temp524.f2; if( _temp588 == 0){ goto _LL531;} else{ goto _LL532;}
_LL532: _LL595: _temp594= _temp524.f1; if( _temp594 ==( void*) Cyc_Absyn_Div){
goto _LL593;} else{ goto _LL534;} _LL593: _temp592= _temp524.f2; if( _temp592 ==
0){ goto _LL533;} else{ goto _LL534;} _LL534: _LL599: _temp598= _temp524.f1; if(
_temp598 ==( void*) Cyc_Absyn_Mod){ goto _LL597;} else{ goto _LL536;} _LL597:
_temp596= _temp524.f2; if( _temp596 == 0){ goto _LL535;} else{ goto _LL536;}
_LL536: _LL603: _temp602= _temp524.f1; if( _temp602 ==( void*) Cyc_Absyn_Plus){
goto _LL601;} else{ goto _LL538;} _LL601: _temp600= _temp524.f2; if( _temp600 ==
1){ goto _LL537;} else{ goto _LL538;} _LL538: _LL607: _temp606= _temp524.f1; if(
_temp606 ==( void*) Cyc_Absyn_Times){ goto _LL605;} else{ goto _LL540;} _LL605:
_temp604= _temp524.f2; if( _temp604 == 1){ goto _LL539;} else{ goto _LL540;}
_LL540: _LL611: _temp610= _temp524.f1; if( _temp610 ==( void*) Cyc_Absyn_Minus){
goto _LL609;} else{ goto _LL542;} _LL609: _temp608= _temp524.f2; if( _temp608 ==
1){ goto _LL541;} else{ goto _LL542;} _LL542: _LL615: _temp614= _temp524.f1; if(
_temp614 ==( void*) Cyc_Absyn_Div){ goto _LL613;} else{ goto _LL544;} _LL613:
_temp612= _temp524.f2; if( _temp612 == 1){ goto _LL543;} else{ goto _LL544;}
_LL544: _LL619: _temp618= _temp524.f1; if( _temp618 ==( void*) Cyc_Absyn_Mod){
goto _LL617;} else{ goto _LL546;} _LL617: _temp616= _temp524.f2; if( _temp616 ==
1){ goto _LL545;} else{ goto _LL546;} _LL546: _LL623: _temp622= _temp524.f1; if(
_temp622 ==( void*) Cyc_Absyn_Eq){ goto _LL621;} else{ goto _LL548;} _LL621:
_temp620= _temp524.f2; goto _LL547; _LL548: _LL627: _temp626= _temp524.f1; if(
_temp626 ==( void*) Cyc_Absyn_Neq){ goto _LL625;} else{ goto _LL550;} _LL625:
_temp624= _temp524.f2; goto _LL549; _LL550: _LL631: _temp630= _temp524.f1; if(
_temp630 ==( void*) Cyc_Absyn_Gt){ goto _LL629;} else{ goto _LL552;} _LL629:
_temp628= _temp524.f2; if( _temp628 == 0){ goto _LL551;} else{ goto _LL552;}
_LL552: _LL635: _temp634= _temp524.f1; if( _temp634 ==( void*) Cyc_Absyn_Lt){
goto _LL633;} else{ goto _LL554;} _LL633: _temp632= _temp524.f2; if( _temp632 ==
0){ goto _LL553;} else{ goto _LL554;} _LL554: _LL639: _temp638= _temp524.f1; if(
_temp638 ==( void*) Cyc_Absyn_Gte){ goto _LL637;} else{ goto _LL556;} _LL637:
_temp636= _temp524.f2; if( _temp636 == 0){ goto _LL555;} else{ goto _LL556;}
_LL556: _LL643: _temp642= _temp524.f1; if( _temp642 ==( void*) Cyc_Absyn_Lte){
goto _LL641;} else{ goto _LL558;} _LL641: _temp640= _temp524.f2; if( _temp640 ==
0){ goto _LL557;} else{ goto _LL558;} _LL558: _LL647: _temp646= _temp524.f1; if(
_temp646 ==( void*) Cyc_Absyn_Gt){ goto _LL645;} else{ goto _LL560;} _LL645:
_temp644= _temp524.f2; if( _temp644 == 1){ goto _LL559;} else{ goto _LL560;}
_LL560: _LL651: _temp650= _temp524.f1; if( _temp650 ==( void*) Cyc_Absyn_Lt){
goto _LL649;} else{ goto _LL562;} _LL649: _temp648= _temp524.f2; if( _temp648 ==
1){ goto _LL561;} else{ goto _LL562;} _LL562: _LL655: _temp654= _temp524.f1; if(
_temp654 ==( void*) Cyc_Absyn_Gte){ goto _LL653;} else{ goto _LL564;} _LL653:
_temp652= _temp524.f2; if( _temp652 == 1){ goto _LL563;} else{ goto _LL564;}
_LL564: _LL659: _temp658= _temp524.f1; if( _temp658 ==( void*) Cyc_Absyn_Lte){
goto _LL657;} else{ goto _LL566;} _LL657: _temp656= _temp524.f2; if( _temp656 ==
1){ goto _LL565;} else{ goto _LL566;} _LL566: _LL663: _temp662= _temp524.f1; if(
_temp662 ==( void*) Cyc_Absyn_Bitand){ goto _LL661;} else{ goto _LL568;} _LL661:
_temp660= _temp524.f2; goto _LL567; _LL568: _LL667: _temp666= _temp524.f1; if(
_temp666 ==( void*) Cyc_Absyn_Bitor){ goto _LL665;} else{ goto _LL570;} _LL665:
_temp664= _temp524.f2; goto _LL569; _LL570: _LL671: _temp670= _temp524.f1; if(
_temp670 ==( void*) Cyc_Absyn_Bitxor){ goto _LL669;} else{ goto _LL572;} _LL669:
_temp668= _temp524.f2; goto _LL571; _LL572: _LL675: _temp674= _temp524.f1; if(
_temp674 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL673;} else{ goto _LL574;}
_LL673: _temp672= _temp524.f2; goto _LL573; _LL574: _LL679: _temp678= _temp524.f1;
if( _temp678 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL677;} else{ goto _LL576;}
_LL677: _temp676= _temp524.f2; goto _LL575; _LL576: _LL683: _temp682= _temp524.f1;
if( _temp682 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL681;} else{ goto _LL578;}
_LL681: _temp680= _temp524.f2; goto _LL577; _LL578: goto _LL579; _LL527: i3= i1
+ i2; use_i3= 1; goto _LL525; _LL529: i3= i1 * i2; use_i3= 1; goto _LL525;
_LL531: i3= i1 - i2; use_i3= 1; goto _LL525; _LL533: i3= i1 / i2; use_i3= 1;
goto _LL525; _LL535: i3= i1 % i2; use_i3= 1; goto _LL525; _LL537: u3= u1 + u2;
use_u3= 1; goto _LL525; _LL539: u3= u1 * u2; use_u3= 1; goto _LL525; _LL541: u3=
u1 - u2; use_u3= 1; goto _LL525; _LL543: u3= u1 / u2; use_u3= 1; goto _LL525;
_LL545: u3= u1 % u2; use_u3= 1; goto _LL525; _LL547: b3= i1 == i2; use_b3= 1;
goto _LL525; _LL549: b3= i1 != i2; use_b3= 1; goto _LL525; _LL551: b3= i1 > i2;
use_b3= 1; goto _LL525; _LL553: b3= i1 < i2; use_b3= 1; goto _LL525; _LL555: b3=
i1 >= i2; use_b3= 1; goto _LL525; _LL557: b3= i1 <= i2; use_b3= 1; goto _LL525;
_LL559: b3= u1 > u2; use_b3= 1; goto _LL525; _LL561: b3= u1 < u2; use_b3= 1;
goto _LL525; _LL563: b3= u1 >= u2; use_b3= 1; goto _LL525; _LL565: b3= u1 <= u2;
use_b3= 1; goto _LL525; _LL567: u3= u1 & u2; use_u3= 1; goto _LL525; _LL569: u3=
u1 | u2; use_u3= 1; goto _LL525; _LL571: u3= u1 ^ u2; use_u3= 1; goto _LL525;
_LL573: u3= u1 << u2; use_u3= 1; goto _LL525; _LL575: u3= u1 >> u2; use_u3= 1;
goto _LL525; _LL577: Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp684=">>> NOT IMPLEMENTED";
struct _tagged_string _temp685; _temp685.curr= _temp684; _temp685.base= _temp684;
_temp685.last_plus_one= _temp684 + 20; _temp685;})); return _temp483; _LL579:
Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp686="bad constant expression";
struct _tagged_string _temp687; _temp687.curr= _temp686; _temp687.base= _temp686;
_temp687.last_plus_one= _temp686 + 24; _temp687;})); return _temp483; _LL525:;}
if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp688=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp688[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp689; _temp689.tag= Cyc_Absyn_Int_c;
_temp689.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp689.f2= i3; _temp689;});
_temp688;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp690=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp690[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp691; _temp691.tag= Cyc_Absyn_Int_c;
_temp691.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp691.f2=( int) u3;
_temp691;}); _temp690;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp692=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp692[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp693; _temp693.tag= Cyc_Absyn_Int_c;
_temp693.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp693.f2= b3? 1: 0; _temp693;});
_temp692;});}( void) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp694=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp694[ 0]=({ struct Cyc_Core_Unreachable_struct _temp695; _temp695.tag= Cyc_Core_Unreachable;
_temp695.f1=({ unsigned char* _temp696="Evexp::eval_const_binop"; struct
_tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 24; _temp697;}); _temp695;}); _temp694;}));}}}}