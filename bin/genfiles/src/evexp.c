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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; int* identity; struct Cyc_Absyn_Conref*
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
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
struct Cyc_Absyn_Tvar* _temp219; struct Cyc_Core_Opt* _temp221; int _temp223;
struct Cyc_Core_Opt* _temp225; struct Cyc_Core_Opt* _temp227; struct Cyc_Core_Opt
_temp229; void* _temp230; struct Cyc_Core_Opt* _temp232; int _temp234; struct
Cyc_Core_Opt* _temp236; struct Cyc_Core_Opt* _temp238; struct Cyc_Absyn_TunionInfo
_temp240; struct Cyc_Absyn_TunionFieldInfo _temp242; struct Cyc_List_List*
_temp244; void* _temp246; struct Cyc_Absyn_Tunionfield* _temp248; struct Cyc_Absyn_Tuniondecl*
_temp250; struct Cyc_Absyn_TunionFieldInfo _temp252; struct Cyc_Absyn_PtrInfo
_temp254; struct Cyc_Absyn_Conref* _temp256; struct Cyc_Absyn_Tqual _temp258;
struct Cyc_Absyn_Conref* _temp260; void* _temp262; void* _temp264; void*
_temp266; void* _temp268; void* _temp270; void* _temp272; void* _temp274; void*
_temp276; void* _temp278; void* _temp280; struct Cyc_Absyn_Exp* _temp282; struct
Cyc_Absyn_Tqual _temp284; void* _temp286; struct Cyc_Absyn_FnInfo _temp288;
struct Cyc_List_List* _temp290; void* _temp292; struct Cyc_Absyn_Structdecl**
_temp294; struct Cyc_List_List* _temp296; struct _tuple0* _temp298; struct Cyc_List_List*
_temp300; struct Cyc_Absyn_Uniondecl** _temp302; struct Cyc_List_List* _temp304;
struct _tuple0* _temp306; struct Cyc_List_List* _temp308; struct Cyc_Absyn_Enumdecl*
_temp310; struct _tuple0* _temp312; struct Cyc_Core_Opt* _temp314; struct Cyc_List_List*
_temp316; struct _tuple0* _temp318; struct Cyc_List_List* _temp320; void*
_temp322; _LL165: if( _temp163 ==( void*) Cyc_Absyn_VoidType){ goto _LL166;}
else{ goto _LL167;} _LL167: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_VarType: 0){ _LL220: _temp219=(( struct Cyc_Absyn_VarType_struct*)
_temp163)->f1; goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_Evar: 0){ _LL228: _temp227=((
struct Cyc_Absyn_Evar_struct*) _temp163)->f1; if( _temp227 == 0){ goto _LL171;}
else{ _temp229=* _temp227; _LL231: _temp230=( void*) _temp229.v; if( _temp230 ==(
void*) Cyc_Absyn_BoxKind){ goto _LL226;} else{ goto _LL171;}} _LL226: _temp225=((
struct Cyc_Absyn_Evar_struct*) _temp163)->f2; goto _LL224; _LL224: _temp223=((
struct Cyc_Absyn_Evar_struct*) _temp163)->f3; goto _LL222; _LL222: _temp221=((
struct Cyc_Absyn_Evar_struct*) _temp163)->f4; goto _LL170;} else{ goto _LL171;}
_LL171: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_Evar:
0){ _LL239: _temp238=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f1; goto
_LL237; _LL237: _temp236=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f2; goto
_LL235; _LL235: _temp234=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f3; goto
_LL233; _LL233: _temp232=(( struct Cyc_Absyn_Evar_struct*) _temp163)->f4; goto
_LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_TunionType: 0){ _LL241: _temp240=(( struct Cyc_Absyn_TunionType_struct*)
_temp163)->f1; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_TunionFieldType: 0){ _LL243:
_temp242=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp163)->f1; _LL247:
_temp246=( void*) _temp242.field_info; if(*(( int*) _temp246) == Cyc_Absyn_KnownTunionfield){
_LL251: _temp250=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp246)->f1;
goto _LL249; _LL249: _temp248=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp246)->f2; goto _LL245;} else{ goto _LL177;} _LL245: _temp244= _temp242.targs;
goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp163 > 4u?*((
int*) _temp163) == Cyc_Absyn_TunionFieldType: 0){ _LL253: _temp252=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp163)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_PointerType: 0){ _LL255: _temp254=((
struct Cyc_Absyn_PointerType_struct*) _temp163)->f1; _LL265: _temp264=( void*)
_temp254.elt_typ; goto _LL263; _LL263: _temp262=( void*) _temp254.rgn_typ; goto
_LL261; _LL261: _temp260= _temp254.nullable; goto _LL259; _LL259: _temp258=
_temp254.tq; goto _LL257; _LL257: _temp256= _temp254.bounds; goto _LL180;} else{
goto _LL181;} _LL181: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) ==
Cyc_Absyn_IntType: 0){ _LL269: _temp268=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL267; _LL267: _temp266=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp266 ==( void*) Cyc_Absyn_B1){ goto _LL182;} else{ goto
_LL183;}} else{ goto _LL183;} _LL183: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL273: _temp272=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL271; _LL271: _temp270=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp270 ==( void*) Cyc_Absyn_B2){ goto _LL184;} else{ goto
_LL185;}} else{ goto _LL185;} _LL185: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL275; _LL275: _temp274=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp274 ==( void*) Cyc_Absyn_B4){ goto _LL186;} else{ goto
_LL187;}} else{ goto _LL187;} _LL187: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL281: _temp280=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL279; _LL279: _temp278=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp278 ==( void*) Cyc_Absyn_B8){ goto _LL188;} else{ goto
_LL189;}} else{ goto _LL189;} _LL189: if( _temp163 ==( void*) Cyc_Absyn_FloatType){
goto _LL190;} else{ goto _LL191;} _LL191: if( _temp163 ==( void*) Cyc_Absyn_DoubleType){
goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int) _temp163 > 4u?*((
int*) _temp163) == Cyc_Absyn_ArrayType: 0){ _LL287: _temp286=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp163)->f1; goto _LL285; _LL285: _temp284=((
struct Cyc_Absyn_ArrayType_struct*) _temp163)->f2; goto _LL283; _LL283: _temp282=((
struct Cyc_Absyn_ArrayType_struct*) _temp163)->f3; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_FnType:
0){ _LL289: _temp288=(( struct Cyc_Absyn_FnType_struct*) _temp163)->f1; goto
_LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_TupleType: 0){ _LL291: _temp290=(( struct Cyc_Absyn_TupleType_struct*)
_temp163)->f1; goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_RgnHandleType: 0){ _LL293:
_temp292=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp163)->f1; goto
_LL200;} else{ goto _LL201;} _LL201: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_StructType: 0){ _LL299: _temp298=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f1; goto _LL297; _LL297: _temp296=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f2; goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_StructType_struct*)
_temp163)->f3; goto _LL202;} else{ goto _LL203;} _LL203: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AnonStructType: 0){ _LL301:
_temp300=(( struct Cyc_Absyn_AnonStructType_struct*) _temp163)->f1; goto _LL204;}
else{ goto _LL205;} _LL205: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_UnionType: 0){ _LL307: _temp306=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f1; goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f2; goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_UnionType_struct*)
_temp163)->f3; goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AnonUnionType: 0){ _LL309:
_temp308=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp163)->f1; goto _LL208;}
else{ goto _LL209;} _LL209: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163)
== Cyc_Absyn_EnumType: 0){ _LL313: _temp312=(( struct Cyc_Absyn_EnumType_struct*)
_temp163)->f1; goto _LL311; _LL311: _temp310=(( struct Cyc_Absyn_EnumType_struct*)
_temp163)->f2; goto _LL210;} else{ goto _LL211;} _LL211: if( _temp163 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_TypedefType: 0){ _LL319: _temp318=((
struct Cyc_Absyn_TypedefType_struct*) _temp163)->f1; goto _LL317; _LL317:
_temp316=(( struct Cyc_Absyn_TypedefType_struct*) _temp163)->f2; goto _LL315;
_LL315: _temp314=(( struct Cyc_Absyn_TypedefType_struct*) _temp163)->f3; goto
_LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_JoinEff: 0){ _LL321: _temp320=(( struct Cyc_Absyn_JoinEff_struct*)
_temp163)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_AccessEff: 0){ _LL323: _temp322=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp163)->f1; goto _LL218;} else{
goto _LL164;} _LL166: Cyc_Tcutil_terr( loc,({ unsigned char* _temp324="cannot apply sizeof to void type";
struct _tagged_string _temp325; _temp325.curr= _temp324; _temp325.base= _temp324;
_temp325.last_plus_one= _temp324 + 33; _temp325;})); return 0u; _LL168: { void*
_temp326=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp219->kind); _LL328: if( _temp326 ==( void*) Cyc_Absyn_BoxKind){ goto _LL329;}
else{ goto _LL330;} _LL330: if( _temp326 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL331;} else{ goto _LL332;} _LL332: if( _temp326 ==( void*) Cyc_Absyn_MemKind){
goto _LL333;} else{ goto _LL334;} _LL334: if( _temp326 ==( void*) Cyc_Absyn_RgnKind){
goto _LL335;} else{ goto _LL336;} _LL336: if( _temp326 ==( void*) Cyc_Absyn_EffKind){
goto _LL337;} else{ goto _LL327;} _LL329: return 4u; _LL331: goto _LL333; _LL333:
Cyc_Tcutil_terr( loc,({ unsigned char* _temp338="cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 49; _temp339;})); return 0u; _LL335: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp340="cannot apply sizeof to a region"; struct
_tagged_string _temp341; _temp341.curr= _temp340; _temp341.base= _temp340;
_temp341.last_plus_one= _temp340 + 32; _temp341;})); return 0u; _LL337: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp342="cannot apply sizeof to an effect"; struct
_tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 33; _temp343;})); return 0u; _LL327:;} _LL170:
return 4u; _LL172: Cyc_Tcutil_terr( loc,({ unsigned char* _temp344="cannot apply sizeof to unknown type";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 36; _temp345;})); return 0u; _LL174: return 4u;
_LL176: return 4u + Cyc_Evexp_szof(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp346=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp346[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp347; _temp347.tag= Cyc_Absyn_TupleType;
_temp347.f1= _temp248->typs; _temp347;}); _temp346;}), loc); _LL178:( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char*
_temp348="unroslved tunionfield type in szof"; struct _tagged_string _temp349;
_temp349.curr= _temp348; _temp349.base= _temp348; _temp349.last_plus_one=
_temp348 + 35; _temp349;}))); _LL180: { void* _temp350=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp256))->v; void*
_temp358; void* _temp360; _LL352: if(( unsigned int) _temp350 > 1u?*(( int*)
_temp350) == Cyc_Absyn_Eq_constr: 0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp350)->f1; if( _temp358 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL353;} else{
goto _LL354;}} else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 > 1u?*((
int*) _temp350) == Cyc_Absyn_Eq_constr: 0){ _LL361: _temp360=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp350)->f1; goto _LL355;} else{ goto _LL356;}
_LL356: goto _LL357; _LL353: return 12u; _LL355: return 4u; _LL357: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp362="bounds information not known for pointer type";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 46; _temp363;})); return 0u; _LL351:;} _LL182:
return 1u; _LL184: return 1u; _LL186: return 1u; _LL188: return 1u; _LL190:
return 4u; _LL192: return 8u; _LL194: if( _temp282 == 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp364="cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 48; _temp365;}));} return Cyc_Evexp_szof(
_temp286, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp282)); _LL196: Cyc_Tcutil_terr( loc,({ unsigned char* _temp366="cannot apply sizeof to function type";
struct _tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 37; _temp367;})); return 0u; _LL198: {
unsigned int s= 0; for( 0; _temp290 != 0; _temp290=(( struct Cyc_List_List*)
_check_null( _temp290))->tl){ s += Cyc_Evexp_szof((*(( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( _temp290))->hd)).f2, loc);} return s;} _LL200:
return 4u; _LL202: if( _temp294 == 0){ return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp368="szof on unchecked StructType";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 29; _temp369;}));}{ struct Cyc_Absyn_Structdecl*
_temp370=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp294)); goto _LL371;
_LL371: if( _temp370->fields == 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp372="cannot apply sizeof to abstract struct type"; struct _tagged_string
_temp373; _temp373.curr= _temp372; _temp373.base= _temp372; _temp373.last_plus_one=
_temp372 + 44; _temp373;}));} _temp300=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp370->fields))->v; goto _LL204;} _LL204: { unsigned int s= 0;
for( 0; _temp300 != 0; _temp300=(( struct Cyc_List_List*) _check_null( _temp300))->tl){
s += Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp300))->hd)->type, loc);} return s;} _LL206: if( _temp302 == 0){
return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp374="szof on unchecked UnionType"; struct _tagged_string
_temp375; _temp375.curr= _temp374; _temp375.base= _temp374; _temp375.last_plus_one=
_temp374 + 28; _temp375;}));}{ struct Cyc_Absyn_Uniondecl* _temp376=*(( struct
Cyc_Absyn_Uniondecl**) _check_null( _temp302)); goto _LL377; _LL377: if(
_temp376->fields == 0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp378="cannot apply sizeof to abstract union type";
struct _tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 43; _temp379;}));} _temp308=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp376->fields))->v; goto _LL208;} _LL208: {
unsigned int s= 0; for( 0; _temp308 != 0; _temp308=(( struct Cyc_List_List*)
_check_null( _temp308))->tl){ unsigned int i= Cyc_Evexp_szof(( void*)(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp308))->hd)->type,
loc); s= s > i? s: i;} return s;} _LL210: return 4u; _LL212: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp380="cannot apply sizoef to a region"; struct
_tagged_string _temp381; _temp381.curr= _temp380; _temp381.base= _temp380;
_temp381.last_plus_one= _temp380 + 32; _temp381;})); return 0u; _LL214: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp382= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp382.last_plus_one - _temp382.curr, _temp382.curr);})); _LL216: goto _LL218;
_LL218: Cyc_Tcutil_terr( loc,({ unsigned char* _temp383="cannot apply sizoef to an effect";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 33; _temp384;})); return 0u; _LL164:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp385=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp385[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp386; _temp386.tag= Cyc_Absyn_Int_c; _temp386.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp386.f2=( int) Cyc_Evexp_szof( t, loc); _temp386;}); _temp385;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp387= cn; unsigned char _temp397; void* _temp399; int
_temp401; void* _temp403; struct _tagged_string _temp405; _LL389: if((
unsigned int) _temp387 > 1u?*(( int*) _temp387) == Cyc_Absyn_Char_c: 0){ _LL400:
_temp399=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp387)->f1; goto _LL398;
_LL398: _temp397=(( struct Cyc_Absyn_Char_c_struct*) _temp387)->f2; goto _LL390;}
else{ goto _LL391;} _LL391: if(( unsigned int) _temp387 > 1u?*(( int*) _temp387)
== Cyc_Absyn_Int_c: 0){ _LL404: _temp403=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp387)->f1; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_Int_c_struct*)
_temp387)->f2; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp387 > 1u?*(( int*) _temp387) == Cyc_Absyn_Float_c: 0){ _LL406: _temp405=((
struct Cyc_Absyn_Float_c_struct*) _temp387)->f1; goto _LL394;} else{ goto _LL395;}
_LL395: goto _LL396; _LL390: return 1; _LL392: return 1; _LL394: return 1;
_LL396: return 0; _LL388:;} void* Cyc_Evexp_eval_const_unprimop( void* p, struct
Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp408=({ struct _tuple3 _temp407; _temp407.f1= p;
_temp407.f2= cn; _temp407;}); void* _temp426; void* _temp428; void* _temp430;
int _temp432; void* _temp434; void* _temp436; void* _temp438; short _temp440;
void* _temp442; void* _temp444; void* _temp446; unsigned char _temp448; void*
_temp450; void* _temp452; void* _temp454; int _temp456; void* _temp458; void*
_temp460; void* _temp462; void* _temp464; void* _temp466; int _temp468; void*
_temp470; void* _temp472; _LL410: _LL429: _temp428= _temp408.f1; if( _temp428 ==(
void*) Cyc_Absyn_Plus){ goto _LL427;} else{ goto _LL412;} _LL427: _temp426=
_temp408.f2; goto _LL411; _LL412: _LL437: _temp436= _temp408.f1; if( _temp436 ==(
void*) Cyc_Absyn_Minus){ goto _LL431;} else{ goto _LL414;} _LL431: _temp430=
_temp408.f2; if(( unsigned int) _temp430 > 1u?*(( int*) _temp430) == Cyc_Absyn_Int_c:
0){ _LL435: _temp434=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp430)->f1;
goto _LL433; _LL433: _temp432=(( struct Cyc_Absyn_Int_c_struct*) _temp430)->f2;
goto _LL413;} else{ goto _LL414;} _LL414: _LL445: _temp444= _temp408.f1; if(
_temp444 ==( void*) Cyc_Absyn_Minus){ goto _LL439;} else{ goto _LL416;} _LL439:
_temp438= _temp408.f2; if(( unsigned int) _temp438 > 1u?*(( int*) _temp438) ==
Cyc_Absyn_Short_c: 0){ _LL443: _temp442=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp438)->f1; goto _LL441; _LL441: _temp440=(( struct Cyc_Absyn_Short_c_struct*)
_temp438)->f2; goto _LL415;} else{ goto _LL416;} _LL416: _LL453: _temp452=
_temp408.f1; if( _temp452 ==( void*) Cyc_Absyn_Minus){ goto _LL447;} else{ goto
_LL418;} _LL447: _temp446= _temp408.f2; if(( unsigned int) _temp446 > 1u?*(( int*)
_temp446) == Cyc_Absyn_Char_c: 0){ _LL451: _temp450=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp446)->f1; goto _LL449; _LL449: _temp448=(( struct Cyc_Absyn_Char_c_struct*)
_temp446)->f2; goto _LL417;} else{ goto _LL418;} _LL418: _LL461: _temp460=
_temp408.f1; if( _temp460 ==( void*) Cyc_Absyn_Not){ goto _LL455;} else{ goto
_LL420;} _LL455: _temp454= _temp408.f2; if(( unsigned int) _temp454 > 1u?*(( int*)
_temp454) == Cyc_Absyn_Int_c: 0){ _LL459: _temp458=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp454)->f1; goto _LL457; _LL457: _temp456=(( struct Cyc_Absyn_Int_c_struct*)
_temp454)->f2; goto _LL419;} else{ goto _LL420;} _LL420: _LL465: _temp464=
_temp408.f1; if( _temp464 ==( void*) Cyc_Absyn_Not){ goto _LL463;} else{ goto
_LL422;} _LL463: _temp462= _temp408.f2; if( _temp462 ==( void*) Cyc_Absyn_Null_c){
goto _LL421;} else{ goto _LL422;} _LL422: _LL473: _temp472= _temp408.f1; if(
_temp472 ==( void*) Cyc_Absyn_Bitnot){ goto _LL467;} else{ goto _LL424;} _LL467:
_temp466= _temp408.f2; if(( unsigned int) _temp466 > 1u?*(( int*) _temp466) ==
Cyc_Absyn_Int_c: 0){ _LL471: _temp470=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp466)->f1; goto _LL469; _LL469: _temp468=(( struct Cyc_Absyn_Int_c_struct*)
_temp466)->f2; goto _LL423;} else{ goto _LL424;} _LL424: goto _LL425; _LL411:
if( ! Cyc_Evexp_is_arith_const( cn)){ Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp474="expecting arithmetic constant"; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 30; _temp475;}));} return cn; _LL413: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp476=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp476[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp477; _temp477.tag= Cyc_Absyn_Int_c;
_temp477.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp477.f2= - _temp432;
_temp477;}); _temp476;}); _LL415: return( void*)({ struct Cyc_Absyn_Short_c_struct*
_temp478=( struct Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp478[ 0]=({ struct Cyc_Absyn_Short_c_struct _temp479; _temp479.tag= Cyc_Absyn_Short_c;
_temp479.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp479.f2= - _temp440;
_temp479;}); _temp478;}); _LL417: return( void*)({ struct Cyc_Absyn_Char_c_struct*
_temp480=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp480[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp481; _temp481.tag= Cyc_Absyn_Char_c;
_temp481.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp481.f2= - _temp448;
_temp481;}); _temp480;}); _LL419: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp482=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp482[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp483; _temp483.tag= Cyc_Absyn_Int_c;
_temp483.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp483.f2= _temp456 == 0? 1:
0; _temp483;}); _temp482;}); _LL421: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp484=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp484[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp485; _temp485.tag= Cyc_Absyn_Int_c;
_temp485.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp485.f2= 1; _temp485;});
_temp484;}); _LL423: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp486=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp486[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp487; _temp487.tag= Cyc_Absyn_Int_c;
_temp487.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp487.f2= ~ _temp468;
_temp487;}); _temp486;}); _LL425: Cyc_Evexp_exp_err( e->loc,({ unsigned char*
_temp488="bad constant expression"; struct _tagged_string _temp489; _temp489.curr=
_temp488; _temp489.base= _temp488; _temp489.last_plus_one= _temp488 + 24;
_temp489;})); return cn; _LL409:;} struct _tuple5{ void* f1; int f2; } ; void*
Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ void* _temp490= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1));
goto _LL491; _LL491: { void* _temp492= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); goto _LL493; _LL493: { void* s1; void* s2; int i1; int i2;{ void* _temp494=
_temp490; int _temp500; void* _temp502; _LL496: if(( unsigned int) _temp494 > 1u?*((
int*) _temp494) == Cyc_Absyn_Int_c: 0){ _LL503: _temp502=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp494)->f1; goto _LL501; _LL501: _temp500=(( struct Cyc_Absyn_Int_c_struct*)
_temp494)->f2; goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL497: s1=
_temp502; i1= _temp500; goto _LL495; _LL499: Cyc_Evexp_exp_err( e1->loc,({
unsigned char* _temp504="bad constant expression"; struct _tagged_string
_temp505; _temp505.curr= _temp504; _temp505.base= _temp504; _temp505.last_plus_one=
_temp504 + 24; _temp505;})); return _temp490; _LL495:;}{ void* _temp506=
_temp492; int _temp512; void* _temp514; _LL508: if(( unsigned int) _temp506 > 1u?*((
int*) _temp506) == Cyc_Absyn_Int_c: 0){ _LL515: _temp514=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp506)->f1; goto _LL513; _LL513: _temp512=(( struct Cyc_Absyn_Int_c_struct*)
_temp506)->f2; goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL509: s2=
_temp514; i2= _temp512; goto _LL507; _LL511: Cyc_Evexp_exp_err( e2->loc,({
unsigned char* _temp516="bad constant expression"; struct _tagged_string
_temp517; _temp517.curr= _temp516; _temp517.base= _temp516; _temp517.last_plus_one=
_temp516 + 24; _temp517;})); return _temp490; _LL507:;}{ void* _temp518= p;
_LL520: if( _temp518 ==( void*) Cyc_Absyn_Div){ goto _LL521;} else{ goto _LL522;}
_LL522: if( _temp518 ==( void*) Cyc_Absyn_Mod){ goto _LL523;} else{ goto _LL524;}
_LL524: goto _LL525; _LL521: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,({
unsigned char* _temp526="division by zero in constant expression"; struct
_tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 40; _temp527;})); return _temp490;} goto
_LL519; _LL523: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,({ unsigned char*
_temp528="division by zero in constant expression"; struct _tagged_string
_temp529; _temp529.curr= _temp528; _temp529.base= _temp528; _temp529.last_plus_one=
_temp528 + 40; _temp529;})); return _temp490;} goto _LL519; _LL525: goto _LL519;
_LL519:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned? 1: s2 ==( void*) Cyc_Absyn_Unsigned;
unsigned int u1=( unsigned int) i1; unsigned int u2=( unsigned int) i2; int i3=
0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int use_u3= 0; int use_b3= 0;{
struct _tuple5 _temp531=({ struct _tuple5 _temp530; _temp530.f1= p; _temp530.f2=
has_u_arg; _temp530;}); int _temp587; void* _temp589; int _temp591; void*
_temp593; int _temp595; void* _temp597; int _temp599; void* _temp601; int
_temp603; void* _temp605; int _temp607; void* _temp609; int _temp611; void*
_temp613; int _temp615; void* _temp617; int _temp619; void* _temp621; int
_temp623; void* _temp625; int _temp627; void* _temp629; int _temp631; void*
_temp633; int _temp635; void* _temp637; int _temp639; void* _temp641; int
_temp643; void* _temp645; int _temp647; void* _temp649; int _temp651; void*
_temp653; int _temp655; void* _temp657; int _temp659; void* _temp661; int
_temp663; void* _temp665; int _temp667; void* _temp669; int _temp671; void*
_temp673; int _temp675; void* _temp677; int _temp679; void* _temp681; int
_temp683; void* _temp685; int _temp687; void* _temp689; _LL533: _LL590: _temp589=
_temp531.f1; if( _temp589 ==( void*) Cyc_Absyn_Plus){ goto _LL588;} else{ goto
_LL535;} _LL588: _temp587= _temp531.f2; if( _temp587 == 0){ goto _LL534;} else{
goto _LL535;} _LL535: _LL594: _temp593= _temp531.f1; if( _temp593 ==( void*) Cyc_Absyn_Times){
goto _LL592;} else{ goto _LL537;} _LL592: _temp591= _temp531.f2; if( _temp591 ==
0){ goto _LL536;} else{ goto _LL537;} _LL537: _LL598: _temp597= _temp531.f1; if(
_temp597 ==( void*) Cyc_Absyn_Minus){ goto _LL596;} else{ goto _LL539;} _LL596:
_temp595= _temp531.f2; if( _temp595 == 0){ goto _LL538;} else{ goto _LL539;}
_LL539: _LL602: _temp601= _temp531.f1; if( _temp601 ==( void*) Cyc_Absyn_Div){
goto _LL600;} else{ goto _LL541;} _LL600: _temp599= _temp531.f2; if( _temp599 ==
0){ goto _LL540;} else{ goto _LL541;} _LL541: _LL606: _temp605= _temp531.f1; if(
_temp605 ==( void*) Cyc_Absyn_Mod){ goto _LL604;} else{ goto _LL543;} _LL604:
_temp603= _temp531.f2; if( _temp603 == 0){ goto _LL542;} else{ goto _LL543;}
_LL543: _LL610: _temp609= _temp531.f1; if( _temp609 ==( void*) Cyc_Absyn_Plus){
goto _LL608;} else{ goto _LL545;} _LL608: _temp607= _temp531.f2; if( _temp607 ==
1){ goto _LL544;} else{ goto _LL545;} _LL545: _LL614: _temp613= _temp531.f1; if(
_temp613 ==( void*) Cyc_Absyn_Times){ goto _LL612;} else{ goto _LL547;} _LL612:
_temp611= _temp531.f2; if( _temp611 == 1){ goto _LL546;} else{ goto _LL547;}
_LL547: _LL618: _temp617= _temp531.f1; if( _temp617 ==( void*) Cyc_Absyn_Minus){
goto _LL616;} else{ goto _LL549;} _LL616: _temp615= _temp531.f2; if( _temp615 ==
1){ goto _LL548;} else{ goto _LL549;} _LL549: _LL622: _temp621= _temp531.f1; if(
_temp621 ==( void*) Cyc_Absyn_Div){ goto _LL620;} else{ goto _LL551;} _LL620:
_temp619= _temp531.f2; if( _temp619 == 1){ goto _LL550;} else{ goto _LL551;}
_LL551: _LL626: _temp625= _temp531.f1; if( _temp625 ==( void*) Cyc_Absyn_Mod){
goto _LL624;} else{ goto _LL553;} _LL624: _temp623= _temp531.f2; if( _temp623 ==
1){ goto _LL552;} else{ goto _LL553;} _LL553: _LL630: _temp629= _temp531.f1; if(
_temp629 ==( void*) Cyc_Absyn_Eq){ goto _LL628;} else{ goto _LL555;} _LL628:
_temp627= _temp531.f2; goto _LL554; _LL555: _LL634: _temp633= _temp531.f1; if(
_temp633 ==( void*) Cyc_Absyn_Neq){ goto _LL632;} else{ goto _LL557;} _LL632:
_temp631= _temp531.f2; goto _LL556; _LL557: _LL638: _temp637= _temp531.f1; if(
_temp637 ==( void*) Cyc_Absyn_Gt){ goto _LL636;} else{ goto _LL559;} _LL636:
_temp635= _temp531.f2; if( _temp635 == 0){ goto _LL558;} else{ goto _LL559;}
_LL559: _LL642: _temp641= _temp531.f1; if( _temp641 ==( void*) Cyc_Absyn_Lt){
goto _LL640;} else{ goto _LL561;} _LL640: _temp639= _temp531.f2; if( _temp639 ==
0){ goto _LL560;} else{ goto _LL561;} _LL561: _LL646: _temp645= _temp531.f1; if(
_temp645 ==( void*) Cyc_Absyn_Gte){ goto _LL644;} else{ goto _LL563;} _LL644:
_temp643= _temp531.f2; if( _temp643 == 0){ goto _LL562;} else{ goto _LL563;}
_LL563: _LL650: _temp649= _temp531.f1; if( _temp649 ==( void*) Cyc_Absyn_Lte){
goto _LL648;} else{ goto _LL565;} _LL648: _temp647= _temp531.f2; if( _temp647 ==
0){ goto _LL564;} else{ goto _LL565;} _LL565: _LL654: _temp653= _temp531.f1; if(
_temp653 ==( void*) Cyc_Absyn_Gt){ goto _LL652;} else{ goto _LL567;} _LL652:
_temp651= _temp531.f2; if( _temp651 == 1){ goto _LL566;} else{ goto _LL567;}
_LL567: _LL658: _temp657= _temp531.f1; if( _temp657 ==( void*) Cyc_Absyn_Lt){
goto _LL656;} else{ goto _LL569;} _LL656: _temp655= _temp531.f2; if( _temp655 ==
1){ goto _LL568;} else{ goto _LL569;} _LL569: _LL662: _temp661= _temp531.f1; if(
_temp661 ==( void*) Cyc_Absyn_Gte){ goto _LL660;} else{ goto _LL571;} _LL660:
_temp659= _temp531.f2; if( _temp659 == 1){ goto _LL570;} else{ goto _LL571;}
_LL571: _LL666: _temp665= _temp531.f1; if( _temp665 ==( void*) Cyc_Absyn_Lte){
goto _LL664;} else{ goto _LL573;} _LL664: _temp663= _temp531.f2; if( _temp663 ==
1){ goto _LL572;} else{ goto _LL573;} _LL573: _LL670: _temp669= _temp531.f1; if(
_temp669 ==( void*) Cyc_Absyn_Bitand){ goto _LL668;} else{ goto _LL575;} _LL668:
_temp667= _temp531.f2; goto _LL574; _LL575: _LL674: _temp673= _temp531.f1; if(
_temp673 ==( void*) Cyc_Absyn_Bitor){ goto _LL672;} else{ goto _LL577;} _LL672:
_temp671= _temp531.f2; goto _LL576; _LL577: _LL678: _temp677= _temp531.f1; if(
_temp677 ==( void*) Cyc_Absyn_Bitxor){ goto _LL676;} else{ goto _LL579;} _LL676:
_temp675= _temp531.f2; goto _LL578; _LL579: _LL682: _temp681= _temp531.f1; if(
_temp681 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL680;} else{ goto _LL581;}
_LL680: _temp679= _temp531.f2; goto _LL580; _LL581: _LL686: _temp685= _temp531.f1;
if( _temp685 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL684;} else{ goto _LL583;}
_LL684: _temp683= _temp531.f2; goto _LL582; _LL583: _LL690: _temp689= _temp531.f1;
if( _temp689 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL688;} else{ goto _LL585;}
_LL688: _temp687= _temp531.f2; goto _LL584; _LL585: goto _LL586; _LL534: i3= i1
+ i2; use_i3= 1; goto _LL532; _LL536: i3= i1 * i2; use_i3= 1; goto _LL532;
_LL538: i3= i1 - i2; use_i3= 1; goto _LL532; _LL540: i3= i1 / i2; use_i3= 1;
goto _LL532; _LL542: i3= i1 % i2; use_i3= 1; goto _LL532; _LL544: u3= u1 + u2;
use_u3= 1; goto _LL532; _LL546: u3= u1 * u2; use_u3= 1; goto _LL532; _LL548: u3=
u1 - u2; use_u3= 1; goto _LL532; _LL550: u3= u1 / u2; use_u3= 1; goto _LL532;
_LL552: u3= u1 % u2; use_u3= 1; goto _LL532; _LL554: b3= i1 == i2; use_b3= 1;
goto _LL532; _LL556: b3= i1 != i2; use_b3= 1; goto _LL532; _LL558: b3= i1 > i2;
use_b3= 1; goto _LL532; _LL560: b3= i1 < i2; use_b3= 1; goto _LL532; _LL562: b3=
i1 >= i2; use_b3= 1; goto _LL532; _LL564: b3= i1 <= i2; use_b3= 1; goto _LL532;
_LL566: b3= u1 > u2; use_b3= 1; goto _LL532; _LL568: b3= u1 < u2; use_b3= 1;
goto _LL532; _LL570: b3= u1 >= u2; use_b3= 1; goto _LL532; _LL572: b3= u1 <= u2;
use_b3= 1; goto _LL532; _LL574: u3= u1 & u2; use_u3= 1; goto _LL532; _LL576: u3=
u1 | u2; use_u3= 1; goto _LL532; _LL578: u3= u1 ^ u2; use_u3= 1; goto _LL532;
_LL580: u3= u1 << u2; use_u3= 1; goto _LL532; _LL582: u3= u1 >> u2; use_u3= 1;
goto _LL532; _LL584: Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp691=">>> NOT IMPLEMENTED";
struct _tagged_string _temp692; _temp692.curr= _temp691; _temp692.base= _temp691;
_temp692.last_plus_one= _temp691 + 20; _temp692;})); return _temp490; _LL586:
Cyc_Evexp_exp_err( e1->loc,({ unsigned char* _temp693="bad constant expression";
struct _tagged_string _temp694; _temp694.curr= _temp693; _temp694.base= _temp693;
_temp694.last_plus_one= _temp693 + 24; _temp694;})); return _temp490; _LL532:;}
if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp695=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp695[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp696; _temp696.tag= Cyc_Absyn_Int_c;
_temp696.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp696.f2= i3; _temp696;});
_temp695;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp697=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp697[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp698; _temp698.tag= Cyc_Absyn_Int_c;
_temp698.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp698.f2=( int) u3;
_temp698;}); _temp697;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp699=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp699[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp700; _temp700.tag= Cyc_Absyn_Int_c;
_temp700.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp700.f2= b3? 1: 0; _temp700;});
_temp699;});}( void) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp701=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp701[ 0]=({ struct Cyc_Core_Unreachable_struct _temp702; _temp702.tag= Cyc_Core_Unreachable;
_temp702.f1=({ unsigned char* _temp703="Evexp::eval_const_binop"; struct
_tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 24; _temp704;}); _temp702;}); _temp701;}));}}}}