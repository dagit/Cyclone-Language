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
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_string
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_string desc;
} ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple0{ void* f1;
struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_exn_typ; extern struct
Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat(
void* p, struct Cyc_Position_Segment* s); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct
Cyc_Absyn_Stmt**); struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void*
f2; } ; extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern int* Cyc_Tcutil_new_tvar_id(); extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer( struct
Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part); struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct Cyc_CfFlowInfo_Place;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; void Cyc_NewControlFlow_cf_check( struct
Cyc_List_List* ds); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser(
te)); s->try_depth= Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
tvs, struct Cyc_List_List* vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp*
where_opt, int new_block){ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
loc, te, tvs); if( new_block){ te2= Cyc_Tcenv_new_block( loc, te2);}{ struct Cyc_Core_Opt*
_temp1=({ struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp0->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp0;});
goto _LL2; _LL2:{ struct Cyc_List_List* _temp3= vs; goto _LL4; _LL4: for( 0;
_temp3 != 0; _temp3=(( struct Cyc_List_List*) _check_null( _temp3))->tl){ te2=
Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp3))->hd);(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp3))->hd)->rgn= _temp1;}} if( where_opt != 0){ Cyc_Tcexp_tcTest(
te2,( struct Cyc_Absyn_Exp*) _check_null( where_opt),({ unsigned char* _temp5="switch clause guard";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 20; _temp6;}));} if( vs != 0){ te2= Cyc_Tcenv_set_encloser( te2, s);}
Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs != 0){ Cyc_NewControlFlow_set_encloser( s,
Cyc_Tcenv_get_encloser( te));}}} void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{
void* _temp7=( void*) s0->r; struct Cyc_Absyn_Exp* _temp49; struct Cyc_Absyn_Stmt*
_temp51; struct Cyc_Absyn_Stmt* _temp53; struct Cyc_Absyn_Exp* _temp55; struct
Cyc_Absyn_Stmt* _temp57; struct Cyc_Absyn_Stmt* _temp59; struct Cyc_Absyn_Exp*
_temp61; struct Cyc_Absyn_Stmt* _temp63; struct _tuple2 _temp65; struct Cyc_Absyn_Stmt*
_temp67; struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Stmt* _temp71; struct
_tuple2 _temp73; struct Cyc_Absyn_Stmt* _temp75; struct Cyc_Absyn_Exp* _temp77;
struct _tuple2 _temp79; struct Cyc_Absyn_Stmt* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct _tuple2 _temp87; struct Cyc_Absyn_Stmt*
_temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Stmt* _temp93; struct
Cyc_Absyn_Stmt* _temp95; struct Cyc_Absyn_Stmt** _temp97; struct Cyc_Absyn_Stmt*
_temp98; struct Cyc_Absyn_Stmt** _temp100; struct Cyc_Absyn_Stmt* _temp101;
struct Cyc_Absyn_Stmt** _temp103; struct _tagged_string* _temp104; struct Cyc_Absyn_Switch_clause**
_temp106; struct Cyc_Absyn_Switch_clause*** _temp108; struct Cyc_List_List*
_temp109; struct Cyc_Absyn_Stmt* _temp111; struct _tagged_string* _temp113;
struct Cyc_List_List* _temp115; struct Cyc_Absyn_Exp* _temp117; struct Cyc_List_List*
_temp119; struct Cyc_Absyn_Exp* _temp121; struct Cyc_List_List* _temp123; struct
Cyc_Absyn_Stmt* _temp125; struct Cyc_Absyn_Stmt* _temp127; struct Cyc_Absyn_Decl*
_temp129; struct Cyc_Absyn_Stmt* _temp131; struct Cyc_Absyn_Vardecl* _temp133;
struct Cyc_Absyn_Tvar* _temp135; struct Cyc_Absyn_Stmt* _temp137; struct Cyc_Absyn_Stmt*
_temp139; _LL9: if( _temp7 ==( void*) Cyc_Absyn_Skip_s){ goto _LL10;} else{ goto
_LL11;} _LL11: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Exp_s:
0){ _LL50: _temp49=(( struct Cyc_Absyn_Exp_s_struct*) _temp7)->f1; goto _LL12;}
else{ goto _LL13;} _LL13: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) ==
Cyc_Absyn_Seq_s: 0){ _LL54: _temp53=(( struct Cyc_Absyn_Seq_s_struct*) _temp7)->f1;
goto _LL52; _LL52: _temp51=(( struct Cyc_Absyn_Seq_s_struct*) _temp7)->f2; goto
_LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Return_s: 0){ _LL56: _temp55=(( struct Cyc_Absyn_Return_s_struct*)
_temp7)->f1; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_IfThenElse_s: 0){ _LL62: _temp61=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f1; goto _LL60; _LL60: _temp59=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f2; goto _LL58; _LL58: _temp57=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f3; goto _LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_While_s: 0){ _LL66: _temp65=(( struct Cyc_Absyn_While_s_struct*)
_temp7)->f1; _LL70: _temp69= _temp65.f1; goto _LL68; _LL68: _temp67= _temp65.f2;
goto _LL64; _LL64: _temp63=(( struct Cyc_Absyn_While_s_struct*) _temp7)->f2;
goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_For_s: 0){ _LL86: _temp85=(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f1; goto _LL80; _LL80: _temp79=(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f2; _LL84: _temp83= _temp79.f1; goto _LL82; _LL82: _temp81= _temp79.f2;
goto _LL74; _LL74: _temp73=(( struct Cyc_Absyn_For_s_struct*) _temp7)->f3; _LL78:
_temp77= _temp73.f1; goto _LL76; _LL76: _temp75= _temp73.f2; goto _LL72; _LL72:
_temp71=(( struct Cyc_Absyn_For_s_struct*) _temp7)->f4; goto _LL22;} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Do_s:
0){ _LL94: _temp93=(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f1; goto _LL88;
_LL88: _temp87=(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f2; _LL92: _temp91=
_temp87.f1; goto _LL90; _LL90: _temp89= _temp87.f2; goto _LL24;} else{ goto
_LL25;} _LL25: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Break_s:
0){ _LL96: _temp95=(( struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; _temp97=&((
struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Continue_s:
0){ _LL99: _temp98=(( struct Cyc_Absyn_Continue_s_struct*) _temp7)->f1; _temp100=&((
struct Cyc_Absyn_Continue_s_struct*) _temp7)->f1; goto _LL28;} else{ goto _LL29;}
_LL29: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Goto_s: 0){
_LL105: _temp104=(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f1; goto _LL102;
_LL102: _temp101=(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; _temp103=&((
struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; goto _LL30;} else{ goto _LL31;}
_LL31: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Fallthru_s:
0){ _LL110: _temp109=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f1; goto
_LL107; _LL107: _temp106=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2;
_temp108=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2; goto _LL32;}
else{ goto _LL33;} _LL33: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) ==
Cyc_Absyn_Label_s: 0){ _LL114: _temp113=(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f1; goto _LL112; _LL112: _temp111=(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Switch_s: 0){ _LL118: _temp117=(( struct Cyc_Absyn_Switch_s_struct*)
_temp7)->f1; goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_Switch_s_struct*)
_temp7)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_SwitchC_s: 0){ _LL122: _temp121=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp7)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp7)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_TryCatch_s: 0){ _LL126: _temp125=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f1; goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f2; goto _LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Decl_s: 0){ _LL130: _temp129=(( struct Cyc_Absyn_Decl_s_struct*)
_temp7)->f1; goto _LL128; _LL128: _temp127=(( struct Cyc_Absyn_Decl_s_struct*)
_temp7)->f2; goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Region_s: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f1; goto _LL134; _LL134: _temp133=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f2; goto _LL132; _LL132: _temp131=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f3; goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Cut_s: 0){ _LL138: _temp137=(( struct Cyc_Absyn_Cut_s_struct*)
_temp7)->f1; goto _LL46;} else{ goto _LL47;} _LL47: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Splice_s: 0){ _LL140: _temp139=(( struct Cyc_Absyn_Splice_s_struct*)
_temp7)->f1; goto _LL48;} else{ goto _LL8;} _LL10: return; _LL12: Cyc_Tcexp_tcExp(
te, 0, _temp49); return; _LL14: { struct Cyc_Tcenv_Tenv* _temp143= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp141=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp141[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp142; _temp142.tag= Cyc_Tcenv_Stmt_j; _temp142.f1= _temp51; _temp142;});
_temp141;})); goto _LL144; _LL144: Cyc_Tcstmt_tcStmt( _temp143, _temp53, 1);
while( 1) { void* _temp145=( void*) _temp51->r; struct Cyc_Absyn_Stmt* _temp151;
struct Cyc_Absyn_Stmt* _temp153; _LL147: if(( unsigned int) _temp145 > 1u?*((
int*) _temp145) == Cyc_Absyn_Seq_s: 0){ _LL154: _temp153=(( struct Cyc_Absyn_Seq_s_struct*)
_temp145)->f1; goto _LL152; _LL152: _temp151=(( struct Cyc_Absyn_Seq_s_struct*)
_temp145)->f2; goto _LL148;} else{ goto _LL149;} _LL149: goto _LL150; _LL148:
Cyc_Tcstmt_decorate_stmt( te, _temp51); _temp143= Cyc_Tcenv_set_next( te,( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp155=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp155[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp156; _temp156.tag= Cyc_Tcenv_Stmt_j; _temp156.f1= _temp151; _temp156;});
_temp155;})); Cyc_Tcstmt_tcStmt( _temp143, _temp153, 1); _temp51= _temp151;
continue; _LL150: goto seq_end; _LL146:;} seq_end: Cyc_Tcstmt_tcStmt( te,
_temp51, 1); return;} _LL16: { void* t= Cyc_Tcenv_return_typ( te); if( _temp55
== 0){ void* _temp157= Cyc_Tcutil_compress( t); _LL159: if( _temp157 ==( void*)
Cyc_Absyn_VoidType){ goto _LL160;} else{ goto _LL161;} _LL161: goto _LL162;
_LL160: goto _LL158; _LL162: Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp163= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp163.last_plus_one - _temp163.curr, _temp163.curr);})); goto _LL158; _LL158:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _check_null( _temp55);
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp164= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct _tagged_string
_temp165= Cyc_Absynpp_typ2string( t); xprintf("returns value of type %.*s but requires %.*s",
_temp164.last_plus_one - _temp164.curr, _temp164.curr, _temp165.last_plus_one -
_temp165.curr, _temp165.curr);}));}{ void* _temp166= Cyc_Tcutil_compress( t);
_LL168: if( _temp166 ==( void*) Cyc_Absyn_VoidType){ goto _LL169;} else{ goto
_LL170;} _LL170: goto _LL171; _LL169: Cyc_Tcutil_terr( s0->loc,({ unsigned char*
_temp172="function returns void expression"; struct _tagged_string _temp173;
_temp173.curr= _temp172; _temp173.base= _temp172; _temp173.last_plus_one=
_temp172 + 33; _temp173;})); goto _LL167; _LL171: goto _LL167; _LL167:;}}
return;} _LL18: Cyc_Tcexp_tcTest( te, _temp61,({ unsigned char* _temp174="if statement";
struct _tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 13; _temp175;})); Cyc_Tcstmt_tcStmt( te,
_temp59, 1); Cyc_Tcstmt_tcStmt( te, _temp57, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt(
te, _temp67); Cyc_Tcexp_tcTest( te, _temp69,({ unsigned char* _temp176="while loop";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 11; _temp177;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp67), _temp63, 1); return; _LL22: Cyc_Tcstmt_decorate_stmt( te, _temp81);
Cyc_Tcstmt_decorate_stmt( te, _temp75); Cyc_Tcexp_tcExp( te, 0, _temp85); Cyc_Tcexp_tcTest(
te, _temp83,({ unsigned char* _temp178="for loop"; struct _tagged_string
_temp179; _temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 9; _temp179;})); te= Cyc_Tcenv_set_in_loop( te, _temp75); Cyc_Tcstmt_tcStmt(
te, _temp71, 1); Cyc_Tcexp_tcExp( te, 0, _temp77); return; _LL24: Cyc_Tcstmt_decorate_stmt(
te, _temp89); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop( te, _temp89), _temp93, 1);
Cyc_Tcexp_tcTest( te, _temp91,({ unsigned char* _temp180="do loop"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 8; _temp181;})); return; _LL26: Cyc_Tcenv_process_break(
te, s0, _temp97); return; _LL28: Cyc_Tcenv_process_continue( te, s0, _temp100);
return; _LL30: Cyc_Tcenv_process_goto( te, s0, _temp104, _temp103); return;
_LL32: { struct _tuple3* _temp182= Cyc_Tcenv_process_fallthru( te, s0, _temp108);
goto _LL183; _LL183: if( _temp182 == 0){ Cyc_Tcutil_terr( s0->loc,({
unsigned char* _temp184="fallthru not in a non-last case"; struct _tagged_string
_temp185; _temp185.curr= _temp184; _temp185.base= _temp184; _temp185.last_plus_one=
_temp184 + 32; _temp185;})); return;}{ struct _tuple3 _temp188; struct Cyc_List_List*
_temp189; struct Cyc_List_List* _temp191; struct Cyc_Absyn_Switch_clause*
_temp193; struct _tuple3* _temp186=( struct _tuple3*) _check_null( _temp182);
_temp188=* _temp186; _LL194: _temp193= _temp188.f1; goto _LL192; _LL192:
_temp191= _temp188.f2; goto _LL190; _LL190: _temp189= _temp188.f3; goto _LL187;
_LL187: { struct Cyc_List_List* instantiation=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_make_inst_var, Cyc_Tcenv_lookup_type_vars(
te), _temp191); _temp189=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*,
void*), struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcutil_substitute, instantiation, _temp189); for( 0; _temp189 != 0? _temp109
!= 0: 0;( _temp189=(( struct Cyc_List_List*) _check_null( _temp189))->tl,
_temp109=(( struct Cyc_List_List*) _check_null( _temp109))->tl)){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp109))->hd);
if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp109))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp189))->hd)){ Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp195=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp109))->hd)->topt))->v);
struct _tagged_string _temp196= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp189))->hd); xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp195.last_plus_one - _temp195.curr, _temp195.curr, _temp196.last_plus_one -
_temp196.curr, _temp196.curr);}));}} if( _temp109 != 0){ Cyc_Tcutil_terr( s0->loc,({
unsigned char* _temp197="too many arguments to explicit fallthru"; struct
_tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 40; _temp198;}));} if( _temp189 != 0){ Cyc_Tcutil_terr(
s0->loc,({ unsigned char* _temp199="too few arguments to explicit fallthru";
struct _tagged_string _temp200; _temp200.curr= _temp199; _temp200.base= _temp199;
_temp200.last_plus_one= _temp199 + 39; _temp200;}));} return;}}} _LL34: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp113, _temp111),({
struct Cyc_Absyn_Tvar* _temp201=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp201->name=({ struct _tagged_string* _temp202=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp202[ 0]=({
struct _tagged_string _temp203=* _temp113; xprintf("`%.*s", _temp203.last_plus_one
- _temp203.curr, _temp203.curr);}); _temp202;}); _temp201->identity= Cyc_Tcutil_new_tvar_id();
_temp201->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp201;})), _temp111, 0); return; _LL36: Cyc_Tcexp_tcExp(
te, 0, _temp117);{ void* _temp204=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp117->topt))->v; goto _LL205; _LL205: te= Cyc_Tcenv_set_in_switch( te); te=
Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp115); for( 0; scs != 0; scs=((
struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Pat* _temp206=((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->pattern;
goto _LL207; _LL207: { struct Cyc_List_List* _temp210; struct Cyc_List_List*
_temp212; struct _tuple5 _temp208= Cyc_Tcpat_tcPat( te, _temp206,( void**)&
_temp204, 0); _LL213: _temp212= _temp208.f1; goto _LL211; _LL211: _temp210=
_temp208.f2; goto _LL209; _LL209: if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp206->topt))->v, _temp204)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc,({ struct _tagged_string
_temp214= Cyc_Absynpp_typ2string( _temp204); struct _tagged_string _temp215= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp206->topt))->v); xprintf("switch on type %.*s, but case expects type %.*s",
_temp214.last_plus_one - _temp214.curr, _temp214.curr, _temp215.last_plus_one -
_temp215.curr, _temp215.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp206);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp216=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp216->v=( void*) _temp210;
_temp216;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc, te, _temp212, _temp210,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp212, _temp210,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd);}}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp115); return;} _LL38: { struct Cyc_List_List* _temp217= 0; goto
_LL218; _LL218: for( 0; _temp119 != 0; _temp119=(( struct Cyc_List_List*)
_check_null( _temp119))->tl){ struct Cyc_Absyn_SwitchC_clause _temp221; struct
Cyc_Position_Segment* _temp222; struct Cyc_Absyn_Stmt* _temp224; struct Cyc_Absyn_Exp*
_temp226; struct Cyc_Absyn_SwitchC_clause* _temp219=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp119))->hd; _temp221=* _temp219; _LL227:
_temp226= _temp221.cnst_exp; goto _LL225; _LL225: _temp224= _temp221.body; goto
_LL223; _LL223: _temp222= _temp221.loc; goto _LL220; _LL220: { struct Cyc_Absyn_Pat*
pat; if( _temp226 != 0){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp226)); pat= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp228=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp228[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp229; _temp229.tag= Cyc_Absyn_Int_p;
_temp229.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp229.f2=( int) Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp226)); _temp229;}); _temp228;}),((
struct Cyc_Absyn_Exp*) _check_null( _temp226))->loc);} else{ pat= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, _temp222);} _temp217=({ struct Cyc_List_List* _temp230=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp230->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp231=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp231->pattern= pat;
_temp231->pat_vars= 0; _temp231->where_clause= 0; _temp231->body= _temp224;
_temp231->loc= _temp222; _temp231;}); _temp230->tl= _temp217; _temp230;});}}(
void*)( s0->r=( void*)(( void*)( Cyc_Absyn_switch_stmt( _temp121,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp217), 0))->r)); Cyc_Tcstmt_tcStmt(
te, s0, new_block); return;} _LL40:( void*)( _temp125->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt(( void*) _temp125->r, _temp125->loc), Cyc_Absyn_skip_stmt(
_temp125->loc), _temp125->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try( te), s0), _temp125, 1); te= Cyc_Tcenv_set_in_switch( te);
te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* _temp232=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp123); goto _LL233; _LL233: for( 0;
_temp232 != 0; _temp232=(( struct Cyc_List_List*) _check_null( _temp232))->tl){
struct Cyc_Absyn_Pat* _temp234=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp232))->hd)->pattern; goto _LL235; _LL235: { struct Cyc_List_List*
_temp238; struct Cyc_List_List* _temp240; struct _tuple5 _temp236= Cyc_Tcpat_tcPat(
te, _temp234,( void**)& Cyc_Absyn_exn_typ, 0); _LL241: _temp240= _temp236.f1;
goto _LL239; _LL239: _temp238= _temp236.f2; goto _LL237; _LL237: if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp234->topt))->v, Cyc_Absyn_exn_typ)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp232))->hd)->loc,({ struct _tagged_string _temp242= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp234->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp242.last_plus_one - _temp242.curr, _temp242.curr);}));} else{ Cyc_Tcpat_check_pat_regions(
te, _temp234);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp232))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp243=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp243->v=( void*)
_temp238; _temp243;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp232))->hd)->loc, te, _temp240, _temp238,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp232))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp232))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp240, _temp238,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp232))->hd);}}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp123); return; _LL42: { struct _tagged_string unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp244=( void*)
_temp129->r; struct Cyc_Absyn_Vardecl* _temp270; int _temp272; int* _temp274;
struct Cyc_Absyn_Exp* _temp275; struct Cyc_Core_Opt* _temp277; struct Cyc_Core_Opt**
_temp279; struct Cyc_Core_Opt* _temp280; struct Cyc_Core_Opt** _temp282; struct
Cyc_Absyn_Pat* _temp283; struct Cyc_List_List* _temp285; struct Cyc_List_List*
_temp287; struct _tagged_string* _temp289; struct Cyc_List_List* _temp291;
struct _tuple0* _temp293; struct Cyc_Absyn_Fndecl* _temp295; struct Cyc_Absyn_Structdecl*
_temp297; struct Cyc_Absyn_Uniondecl* _temp299; struct Cyc_Absyn_Tuniondecl*
_temp301; struct Cyc_Absyn_Enumdecl* _temp303; struct Cyc_Absyn_Typedefdecl*
_temp305; struct Cyc_List_List* _temp307; _LL246: if(*(( int*) _temp244) == Cyc_Absyn_Var_d){
_LL271: _temp270=(( struct Cyc_Absyn_Var_d_struct*) _temp244)->f1; goto _LL247;}
else{ goto _LL248;} _LL248: if(*(( int*) _temp244) == Cyc_Absyn_Let_d){ _LL284:
_temp283=(( struct Cyc_Absyn_Let_d_struct*) _temp244)->f1; goto _LL281; _LL281:
_temp280=(( struct Cyc_Absyn_Let_d_struct*) _temp244)->f2; _temp282=&(( struct
Cyc_Absyn_Let_d_struct*) _temp244)->f2; goto _LL278; _LL278: _temp277=(( struct
Cyc_Absyn_Let_d_struct*) _temp244)->f3; _temp279=&(( struct Cyc_Absyn_Let_d_struct*)
_temp244)->f3; goto _LL276; _LL276: _temp275=(( struct Cyc_Absyn_Let_d_struct*)
_temp244)->f4; goto _LL273; _LL273: _temp272=(( struct Cyc_Absyn_Let_d_struct*)
_temp244)->f5; _temp274=&(( struct Cyc_Absyn_Let_d_struct*) _temp244)->f5; goto
_LL249;} else{ goto _LL250;} _LL250: if(*(( int*) _temp244) == Cyc_Absyn_Letv_d){
_LL286: _temp285=(( struct Cyc_Absyn_Letv_d_struct*) _temp244)->f1; goto _LL251;}
else{ goto _LL252;} _LL252: if(*(( int*) _temp244) == Cyc_Absyn_Namespace_d){
_LL290: _temp289=(( struct Cyc_Absyn_Namespace_d_struct*) _temp244)->f1; goto
_LL288; _LL288: _temp287=(( struct Cyc_Absyn_Namespace_d_struct*) _temp244)->f2;
goto _LL253;} else{ goto _LL254;} _LL254: if(*(( int*) _temp244) == Cyc_Absyn_Using_d){
_LL294: _temp293=(( struct Cyc_Absyn_Using_d_struct*) _temp244)->f1; goto _LL292;
_LL292: _temp291=(( struct Cyc_Absyn_Using_d_struct*) _temp244)->f2; goto _LL255;}
else{ goto _LL256;} _LL256: if(*(( int*) _temp244) == Cyc_Absyn_Fn_d){ _LL296:
_temp295=(( struct Cyc_Absyn_Fn_d_struct*) _temp244)->f1; goto _LL257;} else{
goto _LL258;} _LL258: if(*(( int*) _temp244) == Cyc_Absyn_Struct_d){ _LL298:
_temp297=(( struct Cyc_Absyn_Struct_d_struct*) _temp244)->f1; goto _LL259;}
else{ goto _LL260;} _LL260: if(*(( int*) _temp244) == Cyc_Absyn_Union_d){ _LL300:
_temp299=(( struct Cyc_Absyn_Union_d_struct*) _temp244)->f1; goto _LL261;} else{
goto _LL262;} _LL262: if(*(( int*) _temp244) == Cyc_Absyn_Tunion_d){ _LL302:
_temp301=(( struct Cyc_Absyn_Tunion_d_struct*) _temp244)->f1; goto _LL263;}
else{ goto _LL264;} _LL264: if(*(( int*) _temp244) == Cyc_Absyn_Enum_d){ _LL304:
_temp303=(( struct Cyc_Absyn_Enum_d_struct*) _temp244)->f1; goto _LL265;} else{
goto _LL266;} _LL266: if(*(( int*) _temp244) == Cyc_Absyn_Typedef_d){ _LL306:
_temp305=(( struct Cyc_Absyn_Typedef_d_struct*) _temp244)->f1; goto _LL267;}
else{ goto _LL268;} _LL268: if(*(( int*) _temp244) == Cyc_Absyn_ExternC_d){
_LL308: _temp307=(( struct Cyc_Absyn_ExternC_d_struct*) _temp244)->f1; goto
_LL269;} else{ goto _LL245;} _LL247: { struct Cyc_Absyn_Vardecl _temp311; struct
Cyc_List_List* _temp312; struct Cyc_Core_Opt* _temp314; struct Cyc_Core_Opt**
_temp316; struct Cyc_Absyn_Exp* _temp317; void* _temp319; struct Cyc_Absyn_Tqual
_temp321; struct _tuple0* _temp323; struct _tuple0 _temp325; struct
_tagged_string* _temp326; void* _temp328; void* _temp330; struct Cyc_Absyn_Vardecl*
_temp309= _temp270; _temp311=* _temp309; _LL331: _temp330=( void*) _temp311.sc;
goto _LL324; _LL324: _temp323= _temp311.name; _temp325=* _temp323; _LL329:
_temp328= _temp325.f1; goto _LL327; _LL327: _temp326= _temp325.f2; goto _LL322;
_LL322: _temp321= _temp311.tq; goto _LL320; _LL320: _temp319=( void*) _temp311.type;
goto _LL318; _LL318: _temp317= _temp311.initializer; goto _LL315; _LL315:
_temp314= _temp311.rgn; _temp316=&(* _temp309).rgn; goto _LL313; _LL313:
_temp312= _temp311.attributes; goto _LL310; _LL310: { void* _temp332= Cyc_Tcenv_curr_rgn(
te); goto _LL333; _LL333:* _temp316=({ struct Cyc_Core_Opt* _temp334=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp334->v=( void*)
_temp332; _temp334;});{ void* _temp335= _temp328; struct Cyc_List_List* _temp345;
struct Cyc_List_List* _temp347; _LL337: if( _temp335 ==( void*) Cyc_Absyn_Loc_n){
goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int) _temp335 > 1u?*((
int*) _temp335) == Cyc_Absyn_Rel_n: 0){ _LL346: _temp345=(( struct Cyc_Absyn_Rel_n_struct*)
_temp335)->f1; if( _temp345 == 0){ goto _LL340;} else{ goto _LL341;}} else{ goto
_LL341;} _LL341: if(( unsigned int) _temp335 > 1u?*(( int*) _temp335) == Cyc_Absyn_Abs_n:
0){ _LL348: _temp347=(( struct Cyc_Absyn_Abs_n_struct*) _temp335)->f1; goto
_LL342;} else{ goto _LL343;} _LL343: goto _LL344; _LL338: goto _LL336; _LL340:(*
_temp270->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL336; _LL342:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp349="tcstmt: Abs_n declaration";
struct _tagged_string _temp350; _temp350.curr= _temp349; _temp350.base= _temp349;
_temp350.last_plus_one= _temp349 + 26; _temp350;}))); _LL344: Cyc_Tcutil_terr(
_temp129->loc,({ unsigned char* _temp351="cannot declare a qualified local variable";
struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 42; _temp352;})); goto _LL336; _LL336:;}{
void* _temp353= Cyc_Tcutil_compress( _temp319); struct Cyc_Absyn_Exp* _temp360;
struct Cyc_Absyn_Tqual _temp362; void* _temp364; _LL355: if(( unsigned int)
_temp353 > 4u?*(( int*) _temp353) == Cyc_Absyn_ArrayType: 0){ _LL365: _temp364=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp353)->f1; goto _LL363; _LL363:
_temp362=(( struct Cyc_Absyn_ArrayType_struct*) _temp353)->f2; goto _LL361;
_LL361: _temp360=(( struct Cyc_Absyn_ArrayType_struct*) _temp353)->f3; if(
_temp360 == 0){ goto _LL359;} else{ goto _LL357;}} else{ goto _LL357;} _LL359:
if( _temp270->initializer != 0){ goto _LL356;} else{ goto _LL357;} _LL357: goto
_LL358; _LL356:{ void* _temp366=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp270->initializer))->r; void* _temp376; struct _tagged_string _temp378;
struct Cyc_List_List* _temp380; struct Cyc_Core_Opt* _temp382; struct Cyc_List_List*
_temp384; _LL368: if(*(( int*) _temp366) == Cyc_Absyn_Const_e){ _LL377: _temp376=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp366)->f1; if(( unsigned int)
_temp376 > 1u?*(( int*) _temp376) == Cyc_Absyn_String_c: 0){ _LL379: _temp378=((
struct Cyc_Absyn_String_c_struct*) _temp376)->f1; goto _LL369;} else{ goto
_LL370;}} else{ goto _LL370;} _LL370: if(*(( int*) _temp366) == Cyc_Absyn_UnresolvedMem_e){
_LL383: _temp382=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp366)->f1;
goto _LL381; _LL381: _temp380=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp366)->f2; goto _LL371;} else{ goto _LL372;} _LL372: if(*(( int*) _temp366)
== Cyc_Absyn_Array_e){ _LL385: _temp384=(( struct Cyc_Absyn_Array_e_struct*)
_temp366)->f1; goto _LL373;} else{ goto _LL374;} _LL374: goto _LL375; _LL369:
_temp319=( void*)( _temp270->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp386=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp386[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp387; _temp387.tag= Cyc_Absyn_ArrayType;
_temp387.f1=( void*) _temp364; _temp387.f2= _temp362; _temp387.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp388= _temp378;( unsigned int)(
_temp388.last_plus_one - _temp388.curr);}), 0); _temp387;}); _temp386;}))); goto
_LL367; _LL371: _temp384= _temp380; goto _LL373; _LL373: _temp319=( void*)(
_temp270->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp389=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp389[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp390; _temp390.tag= Cyc_Absyn_ArrayType;
_temp390.f1=( void*) _temp364; _temp390.f2= _temp362; _temp390.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp384), 0); _temp390;}); _temp389;}))); goto _LL367; _LL375: goto _LL367;
_LL367:;} goto _LL354; _LL358: goto _LL354; _LL354:;} Cyc_Tcutil_check_type( s0->loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, _temp319);{
struct Cyc_Tcenv_Tenv* _temp391= Cyc_Tcenv_add_local_var( _temp127->loc, te,
_temp270); goto _LL392; _LL392: if( _temp317 != 0){ Cyc_Tcexp_tcExpInitializer(
_temp391,( void**)& _temp319,( struct Cyc_Absyn_Exp*) _check_null( _temp317));
if( ! Cyc_Tcutil_coerce_assign( _temp391,( struct Cyc_Absyn_Exp*) _check_null(
_temp317), _temp319)){ Cyc_Tcutil_terr( _temp129->loc,({ struct _tagged_string
_temp393=* _temp326; struct _tagged_string _temp394= Cyc_Absynpp_typ2string(
_temp319); struct _tagged_string _temp395= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _check_null(
_temp317))->topt))->v); xprintf("%.*s declared with type %.*s, initialized with type %.*s",
_temp393.last_plus_one - _temp393.curr, _temp393.curr, _temp394.last_plus_one -
_temp394.curr, _temp394.curr, _temp395.last_plus_one - _temp395.curr, _temp395.curr);}));}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp391, s0), _temp127, 0); return;}}}
_LL249: { struct Cyc_Core_Opt* _temp396=* _temp279; goto _LL397; _LL397: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp275);* _temp279= _temp275->topt;{ struct Cyc_List_List* _temp400;
struct Cyc_List_List* _temp402; struct _tuple5 _temp398= Cyc_Tcpat_tcPat( te,
_temp283,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null(
_temp275->topt))->v)), 0); _LL403: _temp402= _temp398.f1; goto _LL401; _LL401:
_temp400= _temp398.f2; goto _LL399; _LL399:* _temp282=({ struct Cyc_Core_Opt*
_temp404=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp404->v=( void*) _temp400; _temp404;}); if( _temp396 != 0? ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp283->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp396))->v): 0){ Cyc_Tcutil_terr( _temp129->loc,({
unsigned char* _temp405="type of pattern does not match declared type"; struct
_tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 45; _temp406;}));} if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp283->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp275->topt))->v)){ Cyc_Tcutil_terr(
_temp129->loc,({ struct _tagged_string _temp407= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp283->topt))->v); struct _tagged_string
_temp408= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp275->topt))->v); xprintf("pattern type %.*s does not match definition type %.*s",
_temp407.last_plus_one - _temp407.curr, _temp407.curr, _temp408.last_plus_one -
_temp408.curr, _temp408.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp283);}* _temp274= Cyc_Tcpat_check_let_pat_exhaustive( _temp283->loc,
_temp283); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp402, _temp400, _temp127,
0, 0); return;}} _LL251: { void* _temp409= Cyc_Tcenv_curr_rgn( te); goto _LL410;
_LL410: { struct Cyc_Tcenv_Tenv* _temp411= te; goto _LL412; _LL412: for( 0;
_temp285 != 0; _temp285=(( struct Cyc_List_List*) _check_null( _temp285))->tl){
struct Cyc_Absyn_Vardecl* _temp413=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp285))->hd; goto _LL414; _LL414: { struct Cyc_Absyn_Vardecl
_temp417; struct Cyc_List_List* _temp418; struct Cyc_Core_Opt* _temp420; struct
Cyc_Core_Opt** _temp422; struct Cyc_Absyn_Exp* _temp423; void* _temp425; struct
Cyc_Absyn_Tqual _temp427; struct _tuple0* _temp429; struct _tuple0 _temp431;
struct _tagged_string* _temp432; void* _temp434; void* _temp436; struct Cyc_Absyn_Vardecl*
_temp415= _temp413; _temp417=* _temp415; _LL437: _temp436=( void*) _temp417.sc;
goto _LL430; _LL430: _temp429= _temp417.name; _temp431=* _temp429; _LL435:
_temp434= _temp431.f1; goto _LL433; _LL433: _temp432= _temp431.f2; goto _LL428;
_LL428: _temp427= _temp417.tq; goto _LL426; _LL426: _temp425=( void*) _temp417.type;
goto _LL424; _LL424: _temp423= _temp417.initializer; goto _LL421; _LL421:
_temp420= _temp417.rgn; _temp422=&(* _temp415).rgn; goto _LL419; _LL419:
_temp418= _temp417.attributes; goto _LL416; _LL416:* _temp422=({ struct Cyc_Core_Opt*
_temp438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp438->v=( void*) _temp409; _temp438;});{ void* _temp439= _temp434; struct
Cyc_List_List* _temp449; struct Cyc_List_List* _temp451; _LL441: if( _temp439 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL442;} else{ goto _LL443;} _LL443: if((
unsigned int) _temp439 > 1u?*(( int*) _temp439) == Cyc_Absyn_Rel_n: 0){ _LL450:
_temp449=(( struct Cyc_Absyn_Rel_n_struct*) _temp439)->f1; if( _temp449 == 0){
goto _LL444;} else{ goto _LL445;}} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp439 > 1u?*(( int*) _temp439) == Cyc_Absyn_Abs_n: 0){ _LL452: _temp451=((
struct Cyc_Absyn_Abs_n_struct*) _temp439)->f1; goto _LL446;} else{ goto _LL447;}
_LL447: goto _LL448; _LL442: goto _LL440; _LL444:(* _temp413->name).f1=( void*)
Cyc_Absyn_Loc_n; goto _LL440; _LL446:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp453="tcstmt: Abs_n declaration";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 26; _temp454;}))); _LL448: Cyc_Tcutil_terr(
_temp129->loc,({ unsigned char* _temp455="cannot declare a qualified local variable";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 42; _temp456;})); goto _LL440; _LL440:;} Cyc_Tcutil_check_type(
s0->loc, _temp411, Cyc_Tcenv_lookup_type_vars( _temp411),( void*) Cyc_Absyn_MemKind,
_temp425); _temp411= Cyc_Tcenv_add_local_var( _temp127->loc, _temp411, _temp413);}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp411, s0), _temp127, 0); return;}}
_LL253: unimp_msg_part=({ unsigned char* _temp457="namespace"; struct
_tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 10; _temp458;}); goto _LL245; _LL255:
unimp_msg_part=({ unsigned char* _temp459="using"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 6; _temp460;}); goto _LL245; _LL257: unimp_msg_part=({ unsigned char*
_temp461="function"; struct _tagged_string _temp462; _temp462.curr= _temp461;
_temp462.base= _temp461; _temp462.last_plus_one= _temp461 + 9; _temp462;}); goto
_LL245; _LL259: unimp_msg_part=({ unsigned char* _temp463="struct"; struct
_tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 7; _temp464;}); goto _LL245; _LL261:
unimp_msg_part=({ unsigned char* _temp465="union"; struct _tagged_string
_temp466; _temp466.curr= _temp465; _temp466.base= _temp465; _temp466.last_plus_one=
_temp465 + 6; _temp466;}); goto _LL245; _LL263: unimp_msg_part=({ unsigned char*
_temp467="[x]tunion"; struct _tagged_string _temp468; _temp468.curr= _temp467;
_temp468.base= _temp467; _temp468.last_plus_one= _temp467 + 10; _temp468;});
goto _LL245; _LL265: unimp_msg_part=({ unsigned char* _temp469="enum"; struct
_tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 5; _temp470;}); goto _LL245; _LL267:
unimp_msg_part=({ unsigned char* _temp471="typedef"; struct _tagged_string
_temp472; _temp472.curr= _temp471; _temp472.base= _temp471; _temp472.last_plus_one=
_temp471 + 8; _temp472;}); goto _LL245; _LL269: unimp_msg_part=({ unsigned char*
_temp473="extern \"C\""; struct _tagged_string _temp474; _temp474.curr= _temp473;
_temp474.base= _temp473; _temp474.last_plus_one= _temp473 + 11; _temp474;});
goto _LL245; _LL245:;}( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp475= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp475.last_plus_one - _temp475.curr, _temp475.curr);})));} _LL44:( void*)(
_temp131->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp131->r, _temp131->loc), Cyc_Absyn_skip_stmt( _temp131->loc), _temp131->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp478; struct Cyc_List_List* _temp479; struct Cyc_Core_Opt* _temp481; struct
Cyc_Core_Opt** _temp483; struct Cyc_Absyn_Exp* _temp484; void* _temp486; struct
Cyc_Absyn_Tqual _temp488; struct _tuple0* _temp490; void* _temp492; struct Cyc_Absyn_Vardecl*
_temp476= _temp133; _temp478=* _temp476; _LL493: _temp492=( void*) _temp478.sc;
goto _LL491; _LL491: _temp490= _temp478.name; goto _LL489; _LL489: _temp488=
_temp478.tq; goto _LL487; _LL487: _temp486=( void*) _temp478.type; goto _LL485;
_LL485: _temp484= _temp478.initializer; goto _LL482; _LL482: _temp481= _temp478.rgn;
_temp483=&(* _temp476).rgn; goto _LL480; _LL480: _temp479= _temp478.attributes;
goto _LL477; _LL477: { void* _temp494= Cyc_Tcenv_curr_rgn( te); goto _LL495;
_LL495: { void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct* _temp502=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp502[ 0]=({ struct Cyc_Absyn_VarType_struct _temp503; _temp503.tag= Cyc_Absyn_VarType;
_temp503.f1= _temp135; _temp503;}); _temp502;});* _temp483=({ struct Cyc_Core_Opt*
_temp496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp496->v=( void*) _temp494; _temp496;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp497=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp497->hd=( void*) _temp135; _temp497->tl= 0;
_temp497;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp486); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp486,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp498=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp498[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp499; _temp499.tag=
Cyc_Absyn_RgnHandleType; _temp499.f1=( void*) rgn_typ; _temp499;}); _temp498;}))){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp500="region stmt: type of region handle is wrong!";
struct _tagged_string _temp501; _temp501.curr= _temp500; _temp501.base= _temp500;
_temp501.last_plus_one= _temp500 + 45; _temp501;}));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var( _temp131->loc, Cyc_Tcenv_enter_try( te), _temp133), s0),
_temp131, 0); return;}}} _LL46:( void) _throw((( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp504="tcStmt: cut is not implemented";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 31; _temp505;}))); _LL48:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp506="tcStmt: splice is not implemented";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 34; _temp507;}))); _LL8:;}}