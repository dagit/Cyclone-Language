 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
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
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern int
Cyc_Evexp_okay_szofarg( void* t); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp0= msg; xprintf("Exp_err: %.*s",
_get_arr_size( _temp0, 1u), _temp0.curr);})));} void* Cyc_Evexp_eval_const_exp(
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
unsigned int) _temp27; _LL26: Cyc_Evexp_exp_err( e->loc, _tag_arr("expecting unsigned int",
sizeof( unsigned char), 23u)); return 0u; _LL22:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp31= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp41; void* _temp43; long long _temp45; void* _temp47; _LL33: if((
unsigned int) _temp31 > 1u?*(( int*) _temp31) == Cyc_Absyn_Int_c: 0){ _LL44:
_temp43=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f1; goto _LL42;
_LL42: _temp41=(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f2; goto _LL34;}
else{ goto _LL35;} _LL35: if(( unsigned int) _temp31 > 1u?*(( int*) _temp31) ==
Cyc_Absyn_LongLong_c: 0){ _LL48: _temp47=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp31)->f1; goto _LL46; _LL46: _temp45=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp31)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if( _temp31 ==( void*) Cyc_Absyn_Null_c){
goto _LL38;} else{ goto _LL39;} _LL39: goto _LL40; _LL34: return _temp41 != 0;
_LL36: return _temp45 != 0; _LL38: return 0; _LL40: Cyc_Evexp_exp_err( e->loc,
_tag_arr("expecting bool", sizeof( unsigned char), 15u)); return 0; _LL32:;}
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; int Cyc_Evexp_okay_szofarg(
void* t){ void* _temp49= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp99;
struct Cyc_Core_Opt* _temp101; int _temp103; struct Cyc_Core_Opt* _temp105;
struct Cyc_Core_Opt* _temp107; struct Cyc_Core_Opt _temp109; void* _temp110;
struct Cyc_Core_Opt* _temp112; int _temp114; struct Cyc_Core_Opt* _temp116;
struct Cyc_Core_Opt* _temp118; struct Cyc_Absyn_TunionInfo _temp120; struct Cyc_Absyn_TunionFieldInfo
_temp122; struct Cyc_List_List* _temp124; void* _temp126; struct Cyc_Absyn_Tunionfield*
_temp128; struct Cyc_Absyn_Tuniondecl* _temp130; struct Cyc_List_List* _temp132;
struct Cyc_Absyn_TunionFieldInfo _temp134; struct Cyc_Absyn_PtrInfo _temp136;
struct Cyc_Absyn_Conref* _temp138; struct Cyc_Absyn_Tqual _temp140; struct Cyc_Absyn_Conref*
_temp142; void* _temp144; void* _temp146; void* _temp148; void* _temp150; struct
Cyc_Absyn_Exp* _temp152; struct Cyc_Absyn_Tqual _temp154; void* _temp156; struct
Cyc_Absyn_FnInfo _temp158; void* _temp160; struct Cyc_Absyn_Structdecl**
_temp162; struct Cyc_List_List* _temp164; struct _tuple0* _temp166; struct Cyc_List_List*
_temp168; struct Cyc_Absyn_Uniondecl** _temp170; struct Cyc_List_List* _temp172;
struct _tuple0* _temp174; struct Cyc_List_List* _temp176; struct Cyc_Absyn_Enumdecl*
_temp178; struct _tuple0* _temp180; struct Cyc_List_List* _temp182; void*
_temp184; struct Cyc_Core_Opt* _temp186; struct Cyc_List_List* _temp188; struct
_tuple0* _temp190; _LL51: if( _temp49 ==( void*) Cyc_Absyn_VoidType){ goto _LL52;}
else{ goto _LL53;} _LL53: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) ==
Cyc_Absyn_VarType: 0){ _LL100: _temp99=(( struct Cyc_Absyn_VarType_struct*)
_temp49)->f1; goto _LL54;} else{ goto _LL55;} _LL55: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_Evar: 0){ _LL108: _temp107=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f1; if( _temp107 == 0){ goto _LL57;} else{ _temp109=* _temp107; _LL111:
_temp110=( void*) _temp109.v; if( _temp110 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL106;} else{ goto _LL57;}} _LL106: _temp105=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f2; goto _LL104; _LL104: _temp103=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f3; goto _LL102; _LL102: _temp101=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f4; goto _LL56;} else{ goto _LL57;} _LL57: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_Evar: 0){ _LL119: _temp118=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f1; goto _LL117; _LL117: _temp116=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f2; goto _LL115; _LL115: _temp114=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f3; goto _LL113; _LL113: _temp112=(( struct Cyc_Absyn_Evar_struct*)
_temp49)->f4; goto _LL58;} else{ goto _LL59;} _LL59: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_TunionType: 0){ _LL121: _temp120=(( struct
Cyc_Absyn_TunionType_struct*) _temp49)->f1; goto _LL60;} else{ goto _LL61;}
_LL61: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) == Cyc_Absyn_TunionFieldType:
0){ _LL123: _temp122=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp49)->f1;
_LL127: _temp126=( void*) _temp122.field_info; if(*(( int*) _temp126) == Cyc_Absyn_KnownTunionfield){
_LL131: _temp130=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp126)->f1;
goto _LL129; _LL129: _temp128=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp126)->f2; goto _LL125;} else{ goto _LL63;} _LL125: _temp124= _temp122.targs;
goto _LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_TupleType: 0){ _LL133: _temp132=(( struct Cyc_Absyn_TupleType_struct*)
_temp49)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_TunionFieldType: 0){ _LL135: _temp134=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp49)->f1; goto _LL66;} else{ goto
_LL67;} _LL67: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) == Cyc_Absyn_PointerType:
0){ _LL137: _temp136=(( struct Cyc_Absyn_PointerType_struct*) _temp49)->f1;
_LL147: _temp146=( void*) _temp136.elt_typ; goto _LL145; _LL145: _temp144=( void*)
_temp136.rgn_typ; goto _LL143; _LL143: _temp142= _temp136.nullable; goto _LL141;
_LL141: _temp140= _temp136.tq; goto _LL139; _LL139: _temp138= _temp136.bounds;
goto _LL68;} else{ goto _LL69;} _LL69: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_IntType: 0){ _LL151: _temp150=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp49)->f1; goto _LL149; _LL149: _temp148=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp49)->f2; goto _LL70;} else{ goto _LL71;} _LL71: if( _temp49 ==( void*) Cyc_Absyn_FloatType){
goto _LL72;} else{ goto _LL73;} _LL73: if( _temp49 ==( void*) Cyc_Absyn_DoubleType){
goto _LL74;} else{ goto _LL75;} _LL75: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_ArrayType: 0){ _LL157: _temp156=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp49)->f1; goto _LL155; _LL155: _temp154=(( struct Cyc_Absyn_ArrayType_struct*)
_temp49)->f2; goto _LL153; _LL153: _temp152=(( struct Cyc_Absyn_ArrayType_struct*)
_temp49)->f3; goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_FnType: 0){ _LL159: _temp158=(( struct Cyc_Absyn_FnType_struct*)
_temp49)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_RgnHandleType: 0){ _LL161: _temp160=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp49)->f1; goto _LL80;} else{ goto
_LL81;} _LL81: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) == Cyc_Absyn_StructType:
0){ _LL167: _temp166=(( struct Cyc_Absyn_StructType_struct*) _temp49)->f1; goto
_LL165; _LL165: _temp164=(( struct Cyc_Absyn_StructType_struct*) _temp49)->f2;
goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_StructType_struct*) _temp49)->f3;
goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_AnonStructType: 0){ _LL169: _temp168=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp49)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_UnionType: 0){ _LL175: _temp174=(( struct
Cyc_Absyn_UnionType_struct*) _temp49)->f1; goto _LL173; _LL173: _temp172=((
struct Cyc_Absyn_UnionType_struct*) _temp49)->f2; goto _LL171; _LL171: _temp170=((
struct Cyc_Absyn_UnionType_struct*) _temp49)->f3; goto _LL86;} else{ goto _LL87;}
_LL87: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) == Cyc_Absyn_AnonUnionType:
0){ _LL177: _temp176=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp49)->f1;
goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_EnumType: 0){ _LL181: _temp180=(( struct Cyc_Absyn_EnumType_struct*)
_temp49)->f1; goto _LL179; _LL179: _temp178=(( struct Cyc_Absyn_EnumType_struct*)
_temp49)->f2; goto _LL90;} else{ goto _LL91;} _LL91: if( _temp49 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp49 > 4u?*(( int*)
_temp49) == Cyc_Absyn_JoinEff: 0){ _LL183: _temp182=(( struct Cyc_Absyn_JoinEff_struct*)
_temp49)->f1; goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp49
> 4u?*(( int*) _temp49) == Cyc_Absyn_AccessEff: 0){ _LL185: _temp184=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp49)->f1; goto _LL96;} else{ goto _LL97;}
_LL97: if(( unsigned int) _temp49 > 4u?*(( int*) _temp49) == Cyc_Absyn_TypedefType:
0){ _LL191: _temp190=(( struct Cyc_Absyn_TypedefType_struct*) _temp49)->f1; goto
_LL189; _LL189: _temp188=(( struct Cyc_Absyn_TypedefType_struct*) _temp49)->f2;
goto _LL187; _LL187: _temp186=(( struct Cyc_Absyn_TypedefType_struct*) _temp49)->f3;
goto _LL98;} else{ goto _LL50;} _LL52: return 0; _LL54: { void* _temp192=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp99->kind); _LL194: if(
_temp192 ==( void*) Cyc_Absyn_BoxKind){ goto _LL195;} else{ goto _LL196;} _LL196:
goto _LL197; _LL195: return 1; _LL197: return 0; _LL193:;} _LL56: return 1;
_LL58: return 0; _LL60: return 1; _LL62: _temp132= _temp128->typs; goto _LL64;
_LL64: for( 0; _temp132 != 0; _temp132=(( struct Cyc_List_List*) _check_null(
_temp132))->tl){ if( ! Cyc_Evexp_okay_szofarg((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp132))->hd)).f2)){ return 0;}} return 1; _LL66: return 0; _LL68: {
void* _temp198=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp138))->v; void* _temp206; void* _temp208;
_LL200: if(( unsigned int) _temp198 > 1u?*(( int*) _temp198) == Cyc_Absyn_Eq_constr:
0){ _LL207: _temp206=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp198)->f1;
if( _temp206 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL201;} else{ goto _LL202;}}
else{ goto _LL202;} _LL202: if(( unsigned int) _temp198 > 1u?*(( int*) _temp198)
== Cyc_Absyn_Eq_constr: 0){ _LL209: _temp208=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp198)->f1; goto _LL203;} else{ goto _LL204;} _LL204: goto _LL205; _LL201:
return 1; _LL203: return 1; _LL205: return 0; _LL199:;} _LL70: return 1; _LL72:
return 1; _LL74: return 1; _LL76: return _temp152 != 0; _LL78: return 0; _LL80:
return 1; _LL82: if( _temp162 == 0){ return(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("szof on unchecked StructType", sizeof( unsigned char), 29u));}{ struct
Cyc_Absyn_Structdecl* _temp210=*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp162)); goto _LL211; _LL211: if( _temp210->fields == 0){ return 0;} _temp168=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp210->fields))->v;
goto _LL84;} _LL84: for( 0; _temp168 != 0; _temp168=(( struct Cyc_List_List*)
_check_null( _temp168))->tl){ if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp168))->hd)->type)){ return 0;}} return
1; _LL86: if( _temp170 == 0){ return(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("szof on unchecked UnionType", sizeof( unsigned char), 28u));}{ struct
Cyc_Absyn_Uniondecl* _temp212=*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp170)); goto _LL213; _LL213: if( _temp212->fields == 0){ return 0;} _temp176=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp212->fields))->v;
goto _LL88;} _LL88: for( 0; _temp176 != 0; _temp176=(( struct Cyc_List_List*)
_check_null( _temp176))->tl){ if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp176))->hd)->type)){ return 0;}} return
1; _LL90: return 1; _LL92: return 0; _LL94: return 0; _LL96: return 0; _LL98:
return(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp214= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_get_arr_size( _temp214, 1u), _temp214.curr);})); _LL50:;} struct _tuple4{ void*
f1; void* f2; } ; void* Cyc_Evexp_eval_const_unprimop( void* p, struct Cyc_Absyn_Exp*
e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); struct
_tuple4 _temp216=({ struct _tuple4 _temp215; _temp215.f1= p; _temp215.f2= cn;
_temp215;}); void* _temp230; void* _temp232; void* _temp234; int _temp236; void*
_temp238; void* _temp240; void* _temp242; int _temp244; void* _temp246; void*
_temp248; void* _temp250; int _temp252; void* _temp254; void* _temp256; void*
_temp258; void* _temp260; _LL218: _LL233: _temp232= _temp216.f1; if( _temp232 ==(
void*) Cyc_Absyn_Plus){ goto _LL231;} else{ goto _LL220;} _LL231: _temp230=
_temp216.f2; goto _LL219; _LL220: _LL241: _temp240= _temp216.f1; if( _temp240 ==(
void*) Cyc_Absyn_Minus){ goto _LL235;} else{ goto _LL222;} _LL235: _temp234=
_temp216.f2; if(( unsigned int) _temp234 > 1u?*(( int*) _temp234) == Cyc_Absyn_Int_c:
0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp234)->f1;
goto _LL237; _LL237: _temp236=(( struct Cyc_Absyn_Int_c_struct*) _temp234)->f2;
goto _LL221;} else{ goto _LL222;} _LL222: _LL249: _temp248= _temp216.f1; if(
_temp248 ==( void*) Cyc_Absyn_Bitnot){ goto _LL243;} else{ goto _LL224;} _LL243:
_temp242= _temp216.f2; if(( unsigned int) _temp242 > 1u?*(( int*) _temp242) ==
Cyc_Absyn_Int_c: 0){ _LL247: _temp246=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp242)->f1; goto _LL245; _LL245: _temp244=(( struct Cyc_Absyn_Int_c_struct*)
_temp242)->f2; goto _LL223;} else{ goto _LL224;} _LL224: _LL257: _temp256=
_temp216.f1; if( _temp256 ==( void*) Cyc_Absyn_Not){ goto _LL251;} else{ goto
_LL226;} _LL251: _temp250= _temp216.f2; if(( unsigned int) _temp250 > 1u?*(( int*)
_temp250) == Cyc_Absyn_Int_c: 0){ _LL255: _temp254=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp250)->f1; goto _LL253; _LL253: _temp252=(( struct Cyc_Absyn_Int_c_struct*)
_temp250)->f2; goto _LL225;} else{ goto _LL226;} _LL226: _LL261: _temp260=
_temp216.f1; if( _temp260 ==( void*) Cyc_Absyn_Not){ goto _LL259;} else{ goto
_LL228;} _LL259: _temp258= _temp216.f2; if( _temp258 ==( void*) Cyc_Absyn_Null_c){
goto _LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL219: return cn; _LL221:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp262=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp262[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp263; _temp263.tag= Cyc_Absyn_Int_c; _temp263.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp263.f2= - _temp236; _temp263;}); _temp262;}); _LL223: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp264=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp264[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp265; _temp265.tag= Cyc_Absyn_Int_c; _temp265.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp265.f2= ~ _temp244; _temp265;}); _temp264;}); _LL225: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp266=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp266[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp267; _temp267.tag= Cyc_Absyn_Int_c; _temp267.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp267.f2= _temp252 == 0? 1: 0; _temp267;}); _temp266;}); _LL227: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp268=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp268[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp269; _temp269.tag= Cyc_Absyn_Int_c; _temp269.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp269.f2= 1; _temp269;}); _temp268;}); _LL229: Cyc_Evexp_exp_err( e->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return cn;
_LL217:;} struct _tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp270=
Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1)); goto _LL271; _LL271: {
void* _temp272= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e2)); goto
_LL273; _LL273: { void* s1; void* s2; int i1; int i2;{ void* _temp274= _temp270;
int _temp280; void* _temp282; _LL276: if(( unsigned int) _temp274 > 1u?*(( int*)
_temp274) == Cyc_Absyn_Int_c: 0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp274)->f1; goto _LL281; _LL281: _temp280=(( struct Cyc_Absyn_Int_c_struct*)
_temp274)->f2; goto _LL277;} else{ goto _LL278;} _LL278: goto _LL279; _LL277: s1=
_temp282; i1= _temp280; goto _LL275; _LL279: Cyc_Evexp_exp_err( e1->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp270; _LL275:;}{ void* _temp284= _temp272; int _temp290; void* _temp292;
_LL286: if(( unsigned int) _temp284 > 1u?*(( int*) _temp284) == Cyc_Absyn_Int_c:
0){ _LL293: _temp292=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp284)->f1;
goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Int_c_struct*) _temp284)->f2;
goto _LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: s2= _temp292; i2=
_temp290; goto _LL285; _LL289: Cyc_Evexp_exp_err( e2->loc, _tag_arr("bad constant expression",
sizeof( unsigned char), 24u)); return _temp270; _LL285:;}{ void* _temp294= p;
_LL296: if( _temp294 ==( void*) Cyc_Absyn_Div){ goto _LL297;} else{ goto _LL298;}
_LL298: if( _temp294 ==( void*) Cyc_Absyn_Mod){ goto _LL299;} else{ goto _LL300;}
_LL300: goto _LL301; _LL297: goto _LL299; _LL299: if( i2 == 0){ Cyc_Evexp_exp_err(
e2->loc, _tag_arr("division by zero in constant expression", sizeof(
unsigned char), 40u)); return _temp270;} goto _LL295; _LL301: goto _LL295;
_LL295:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned? 1: s2 ==( void*) Cyc_Absyn_Unsigned;
unsigned int u1=( unsigned int) i1; unsigned int u2=( unsigned int) i2; int i3=
0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int use_u3= 0; int use_b3= 0;{
struct _tuple5 _temp303=({ struct _tuple5 _temp302; _temp302.f1= p; _temp302.f2=
has_u_arg; _temp302;}); int _temp359; void* _temp361; int _temp363; void*
_temp365; int _temp367; void* _temp369; int _temp371; void* _temp373; int
_temp375; void* _temp377; int _temp379; void* _temp381; int _temp383; void*
_temp385; int _temp387; void* _temp389; int _temp391; void* _temp393; int
_temp395; void* _temp397; int _temp399; void* _temp401; int _temp403; void*
_temp405; int _temp407; void* _temp409; int _temp411; void* _temp413; int
_temp415; void* _temp417; int _temp419; void* _temp421; int _temp423; void*
_temp425; int _temp427; void* _temp429; int _temp431; void* _temp433; int
_temp435; void* _temp437; int _temp439; void* _temp441; int _temp443; void*
_temp445; int _temp447; void* _temp449; int _temp451; void* _temp453; int
_temp455; void* _temp457; int _temp459; void* _temp461; _LL305: _LL362: _temp361=
_temp303.f1; if( _temp361 ==( void*) Cyc_Absyn_Plus){ goto _LL360;} else{ goto
_LL307;} _LL360: _temp359= _temp303.f2; if( _temp359 == 0){ goto _LL306;} else{
goto _LL307;} _LL307: _LL366: _temp365= _temp303.f1; if( _temp365 ==( void*) Cyc_Absyn_Times){
goto _LL364;} else{ goto _LL309;} _LL364: _temp363= _temp303.f2; if( _temp363 ==
0){ goto _LL308;} else{ goto _LL309;} _LL309: _LL370: _temp369= _temp303.f1; if(
_temp369 ==( void*) Cyc_Absyn_Minus){ goto _LL368;} else{ goto _LL311;} _LL368:
_temp367= _temp303.f2; if( _temp367 == 0){ goto _LL310;} else{ goto _LL311;}
_LL311: _LL374: _temp373= _temp303.f1; if( _temp373 ==( void*) Cyc_Absyn_Div){
goto _LL372;} else{ goto _LL313;} _LL372: _temp371= _temp303.f2; if( _temp371 ==
0){ goto _LL312;} else{ goto _LL313;} _LL313: _LL378: _temp377= _temp303.f1; if(
_temp377 ==( void*) Cyc_Absyn_Mod){ goto _LL376;} else{ goto _LL315;} _LL376:
_temp375= _temp303.f2; if( _temp375 == 0){ goto _LL314;} else{ goto _LL315;}
_LL315: _LL382: _temp381= _temp303.f1; if( _temp381 ==( void*) Cyc_Absyn_Plus){
goto _LL380;} else{ goto _LL317;} _LL380: _temp379= _temp303.f2; if( _temp379 ==
1){ goto _LL316;} else{ goto _LL317;} _LL317: _LL386: _temp385= _temp303.f1; if(
_temp385 ==( void*) Cyc_Absyn_Times){ goto _LL384;} else{ goto _LL319;} _LL384:
_temp383= _temp303.f2; if( _temp383 == 1){ goto _LL318;} else{ goto _LL319;}
_LL319: _LL390: _temp389= _temp303.f1; if( _temp389 ==( void*) Cyc_Absyn_Minus){
goto _LL388;} else{ goto _LL321;} _LL388: _temp387= _temp303.f2; if( _temp387 ==
1){ goto _LL320;} else{ goto _LL321;} _LL321: _LL394: _temp393= _temp303.f1; if(
_temp393 ==( void*) Cyc_Absyn_Div){ goto _LL392;} else{ goto _LL323;} _LL392:
_temp391= _temp303.f2; if( _temp391 == 1){ goto _LL322;} else{ goto _LL323;}
_LL323: _LL398: _temp397= _temp303.f1; if( _temp397 ==( void*) Cyc_Absyn_Mod){
goto _LL396;} else{ goto _LL325;} _LL396: _temp395= _temp303.f2; if( _temp395 ==
1){ goto _LL324;} else{ goto _LL325;} _LL325: _LL402: _temp401= _temp303.f1; if(
_temp401 ==( void*) Cyc_Absyn_Eq){ goto _LL400;} else{ goto _LL327;} _LL400:
_temp399= _temp303.f2; goto _LL326; _LL327: _LL406: _temp405= _temp303.f1; if(
_temp405 ==( void*) Cyc_Absyn_Neq){ goto _LL404;} else{ goto _LL329;} _LL404:
_temp403= _temp303.f2; goto _LL328; _LL329: _LL410: _temp409= _temp303.f1; if(
_temp409 ==( void*) Cyc_Absyn_Gt){ goto _LL408;} else{ goto _LL331;} _LL408:
_temp407= _temp303.f2; if( _temp407 == 0){ goto _LL330;} else{ goto _LL331;}
_LL331: _LL414: _temp413= _temp303.f1; if( _temp413 ==( void*) Cyc_Absyn_Lt){
goto _LL412;} else{ goto _LL333;} _LL412: _temp411= _temp303.f2; if( _temp411 ==
0){ goto _LL332;} else{ goto _LL333;} _LL333: _LL418: _temp417= _temp303.f1; if(
_temp417 ==( void*) Cyc_Absyn_Gte){ goto _LL416;} else{ goto _LL335;} _LL416:
_temp415= _temp303.f2; if( _temp415 == 0){ goto _LL334;} else{ goto _LL335;}
_LL335: _LL422: _temp421= _temp303.f1; if( _temp421 ==( void*) Cyc_Absyn_Lte){
goto _LL420;} else{ goto _LL337;} _LL420: _temp419= _temp303.f2; if( _temp419 ==
0){ goto _LL336;} else{ goto _LL337;} _LL337: _LL426: _temp425= _temp303.f1; if(
_temp425 ==( void*) Cyc_Absyn_Gt){ goto _LL424;} else{ goto _LL339;} _LL424:
_temp423= _temp303.f2; if( _temp423 == 1){ goto _LL338;} else{ goto _LL339;}
_LL339: _LL430: _temp429= _temp303.f1; if( _temp429 ==( void*) Cyc_Absyn_Lt){
goto _LL428;} else{ goto _LL341;} _LL428: _temp427= _temp303.f2; if( _temp427 ==
1){ goto _LL340;} else{ goto _LL341;} _LL341: _LL434: _temp433= _temp303.f1; if(
_temp433 ==( void*) Cyc_Absyn_Gte){ goto _LL432;} else{ goto _LL343;} _LL432:
_temp431= _temp303.f2; if( _temp431 == 1){ goto _LL342;} else{ goto _LL343;}
_LL343: _LL438: _temp437= _temp303.f1; if( _temp437 ==( void*) Cyc_Absyn_Lte){
goto _LL436;} else{ goto _LL345;} _LL436: _temp435= _temp303.f2; if( _temp435 ==
1){ goto _LL344;} else{ goto _LL345;} _LL345: _LL442: _temp441= _temp303.f1; if(
_temp441 ==( void*) Cyc_Absyn_Bitand){ goto _LL440;} else{ goto _LL347;} _LL440:
_temp439= _temp303.f2; goto _LL346; _LL347: _LL446: _temp445= _temp303.f1; if(
_temp445 ==( void*) Cyc_Absyn_Bitor){ goto _LL444;} else{ goto _LL349;} _LL444:
_temp443= _temp303.f2; goto _LL348; _LL349: _LL450: _temp449= _temp303.f1; if(
_temp449 ==( void*) Cyc_Absyn_Bitxor){ goto _LL448;} else{ goto _LL351;} _LL448:
_temp447= _temp303.f2; goto _LL350; _LL351: _LL454: _temp453= _temp303.f1; if(
_temp453 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL452;} else{ goto _LL353;}
_LL452: _temp451= _temp303.f2; goto _LL352; _LL353: _LL458: _temp457= _temp303.f1;
if( _temp457 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL456;} else{ goto _LL355;}
_LL456: _temp455= _temp303.f2; goto _LL354; _LL355: _LL462: _temp461= _temp303.f1;
if( _temp461 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL460;} else{ goto _LL357;}
_LL460: _temp459= _temp303.f2; goto _LL356; _LL357: goto _LL358; _LL306: i3= i1
+ i2; use_i3= 1; goto _LL304; _LL308: i3= i1 * i2; use_i3= 1; goto _LL304;
_LL310: i3= i1 - i2; use_i3= 1; goto _LL304; _LL312: i3= i1 / i2; use_i3= 1;
goto _LL304; _LL314: i3= i1 % i2; use_i3= 1; goto _LL304; _LL316: u3= u1 + u2;
use_u3= 1; goto _LL304; _LL318: u3= u1 * u2; use_u3= 1; goto _LL304; _LL320: u3=
u1 - u2; use_u3= 1; goto _LL304; _LL322: u3= u1 / u2; use_u3= 1; goto _LL304;
_LL324: u3= u1 % u2; use_u3= 1; goto _LL304; _LL326: b3= i1 == i2; use_b3= 1;
goto _LL304; _LL328: b3= i1 != i2; use_b3= 1; goto _LL304; _LL330: b3= i1 > i2;
use_b3= 1; goto _LL304; _LL332: b3= i1 < i2; use_b3= 1; goto _LL304; _LL334: b3=
i1 >= i2; use_b3= 1; goto _LL304; _LL336: b3= i1 <= i2; use_b3= 1; goto _LL304;
_LL338: b3= u1 > u2; use_b3= 1; goto _LL304; _LL340: b3= u1 < u2; use_b3= 1;
goto _LL304; _LL342: b3= u1 >= u2; use_b3= 1; goto _LL304; _LL344: b3= u1 <= u2;
use_b3= 1; goto _LL304; _LL346: u3= u1 & u2; use_u3= 1; goto _LL304; _LL348: u3=
u1 | u2; use_u3= 1; goto _LL304; _LL350: u3= u1 ^ u2; use_u3= 1; goto _LL304;
_LL352: u3= u1 << u2; use_u3= 1; goto _LL304; _LL354: u3= u1 >> u2; use_u3= 1;
goto _LL304; _LL356: Cyc_Evexp_exp_err( e1->loc, _tag_arr(">>> NOT IMPLEMENTED",
sizeof( unsigned char), 20u)); return _temp270; _LL358: Cyc_Evexp_exp_err( e1->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp270; _LL304:;} if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp463=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp463[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp464; _temp464.tag= Cyc_Absyn_Int_c;
_temp464.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp464.f2= i3; _temp464;});
_temp463;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp465=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp465[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp466; _temp466.tag= Cyc_Absyn_Int_c;
_temp466.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp466.f2=( int) u3;
_temp466;}); _temp465;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp467=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp467[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp468; _temp468.tag= Cyc_Absyn_Int_c;
_temp468.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp468.f2= b3? 1: 0; _temp468;});
_temp467;});}( int) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp469=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp469[ 0]=({ struct Cyc_Core_Unreachable_struct _temp470; _temp470.tag= Cyc_Core_Unreachable;
_temp470.f1= _tag_arr("Evexp::eval_const_binop", sizeof( unsigned char), 24u);
_temp470;}); _temp469;}));}}}} void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp*
e){ void* cn;{ void* _temp471=( void*) e->r; void* _temp491; struct Cyc_Absyn_Exp*
_temp493; struct Cyc_Absyn_Exp* _temp495; struct Cyc_Absyn_Exp* _temp497; struct
Cyc_List_List* _temp499; void* _temp501; void* _temp503; struct Cyc_Absyn_Exp*
_temp505; struct _tagged_arr* _temp507; void* _temp509; struct Cyc_Absyn_Exp*
_temp511; void* _temp513; struct Cyc_Absyn_Enumfield* _temp515; struct Cyc_Absyn_Enumdecl*
_temp517; struct _tuple0* _temp519; _LL473: if(*(( int*) _temp471) == Cyc_Absyn_Const_e){
_LL492: _temp491=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp471)->f1;
goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp471) == Cyc_Absyn_Conditional_e){
_LL498: _temp497=(( struct Cyc_Absyn_Conditional_e_struct*) _temp471)->f1; goto
_LL496; _LL496: _temp495=(( struct Cyc_Absyn_Conditional_e_struct*) _temp471)->f2;
goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp471)->f3; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp471)
== Cyc_Absyn_Primop_e){ _LL502: _temp501=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp471)->f1; goto _LL500; _LL500: _temp499=(( struct Cyc_Absyn_Primop_e_struct*)
_temp471)->f2; goto _LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp471)
== Cyc_Absyn_Sizeoftyp_e){ _LL504: _temp503=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp471)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(*(( int*) _temp471)
== Cyc_Absyn_Sizeofexp_e){ _LL506: _temp505=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp471)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(*(( int*) _temp471)
== Cyc_Absyn_Offsetof_e){ _LL510: _temp509=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp471)->f1; goto _LL508; _LL508: _temp507=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp471)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if(*(( int*) _temp471)
== Cyc_Absyn_Cast_e){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp471)->f1; goto _LL512; _LL512: _temp511=(( struct Cyc_Absyn_Cast_e_struct*)
_temp471)->f2; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp471)
== Cyc_Absyn_Enum_e){ _LL520: _temp519=(( struct Cyc_Absyn_Enum_e_struct*)
_temp471)->f1; goto _LL518; _LL518: _temp517=(( struct Cyc_Absyn_Enum_e_struct*)
_temp471)->f2; goto _LL516; _LL516: _temp515=(( struct Cyc_Absyn_Enum_e_struct*)
_temp471)->f3; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL474:
return _temp491; _LL476: cn= Cyc_Evexp_eval_const_bool_exp( _temp497)? Cyc_Evexp_eval_const_exp(
_temp495): Cyc_Evexp_eval_const_exp( _temp493); goto _LL472; _LL478: if(
_temp499 == 0){ Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression (no args to primop)",
sizeof( unsigned char), 42u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp521=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp521[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp522; _temp522.tag= Cyc_Absyn_Int_c;
_temp522.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp522.f2= 0; _temp522;});
_temp521;});} if((( struct Cyc_List_List*) _check_null( _temp499))->tl == 0){ cn=
Cyc_Evexp_eval_const_unprimop( _temp501,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp499))->hd);} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp499))->tl))->tl != 0){ Cyc_Evexp_exp_err(
e->loc, _tag_arr("bad static expression (too many args to primop)", sizeof(
unsigned char), 48u)); return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp523=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp523[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp524; _temp524.tag= Cyc_Absyn_Int_c;
_temp524.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp524.f2= 0; _temp524;});
_temp523;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp501,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp499))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp499))->tl))->hd);}} goto _LL472; _LL480: goto _LL482; _LL482: Cyc_Evexp_exp_err(
e->loc, _tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 62u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp525=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp525[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp526; _temp526.tag= Cyc_Absyn_Int_c;
_temp526.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp526.f2= 0; _temp526;});
_temp525;}); _LL484: Cyc_Evexp_exp_err( e->loc, _tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 64u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp527=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp527[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp528; _temp528.tag= Cyc_Absyn_Int_c;
_temp528.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp528.f2= 0; _temp528;});
_temp527;}); _LL486: cn= Cyc_Evexp_eval_const_exp( _temp511);{ struct _tuple4
_temp530=({ struct _tuple4 _temp529; _temp529.f1= Cyc_Tcutil_compress( _temp513);
_temp529.f2= cn; _temp529;}); void* _temp540; unsigned char _temp542; void*
_temp544; void* _temp546; void* _temp548; void* _temp550; void* _temp552; short
_temp554; void* _temp556; void* _temp558; void* _temp560; void* _temp562; void*
_temp564; int _temp566; void* _temp568; void* _temp570; void* _temp572; void*
_temp574; _LL532: _LL547: _temp546= _temp530.f1; if(( unsigned int) _temp546 > 4u?*((
int*) _temp546) == Cyc_Absyn_IntType: 0){ _LL551: _temp550=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp546)->f1; goto _LL549; _LL549: _temp548=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp546)->f2; goto _LL541;} else{ goto _LL534;} _LL541: _temp540= _temp530.f2;
if(( unsigned int) _temp540 > 1u?*(( int*) _temp540) == Cyc_Absyn_Char_c: 0){
_LL545: _temp544=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp540)->f1; goto
_LL543; _LL543: _temp542=(( struct Cyc_Absyn_Char_c_struct*) _temp540)->f2; goto
_LL533;} else{ goto _LL534;} _LL534: _LL559: _temp558= _temp530.f1; if((
unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_IntType: 0){ _LL563:
_temp562=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp558)->f1; goto _LL561;
_LL561: _temp560=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp558)->f2;
goto _LL553;} else{ goto _LL536;} _LL553: _temp552= _temp530.f2; if((
unsigned int) _temp552 > 1u?*(( int*) _temp552) == Cyc_Absyn_Short_c: 0){ _LL557:
_temp556=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp552)->f1; goto _LL555;
_LL555: _temp554=(( struct Cyc_Absyn_Short_c_struct*) _temp552)->f2; goto _LL535;}
else{ goto _LL536;} _LL536: _LL571: _temp570= _temp530.f1; if(( unsigned int)
_temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_IntType: 0){ _LL575: _temp574=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp570)->f1; goto _LL573; _LL573:
_temp572=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp570)->f2; goto _LL565;}
else{ goto _LL538;} _LL565: _temp564= _temp530.f2; if(( unsigned int) _temp564 >
1u?*(( int*) _temp564) == Cyc_Absyn_Int_c: 0){ _LL569: _temp568=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp564)->f1; goto _LL567; _LL567: _temp566=((
struct Cyc_Absyn_Int_c_struct*) _temp564)->f2; goto _LL537;} else{ goto _LL538;}
_LL538: goto _LL539; _LL533: _temp562= _temp550; _temp560= _temp548; _temp556=
_temp544; _temp554=( short) _temp542; goto _LL535; _LL535: _temp574= _temp562;
_temp572= _temp560; _temp568= _temp556; _temp566=( int) _temp554; goto _LL537;
_LL537: if( _temp574 != _temp568){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp576=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp576[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp577; _temp577.tag= Cyc_Absyn_Int_c;
_temp577.f1=( void*) _temp574; _temp577.f2= _temp566; _temp577;}); _temp576;});}
goto _LL531; _LL539: Cyc_Evexp_exp_err( e->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp578= Cyc_Absynpp_typ2string( _temp513); xprintf("eval_const: cannot cast to %.*s",
_get_arr_size( _temp578, 1u), _temp578.curr);})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp579=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp579[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp580; _temp580.tag= Cyc_Absyn_Int_c;
_temp580.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp580.f2= 0; _temp580;});
_temp579;}); _LL531:;} goto _LL472; _LL488: return Cyc_Evexp_eval_const_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*) _check_null(
_temp515))->tag)); _LL490: Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression",
sizeof( unsigned char), 22u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp581=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp581[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp582; _temp582.tag= Cyc_Absyn_Int_c;
_temp582.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp582.f2= 0; _temp582;});
_temp581;}); _LL472:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp583=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp583[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp584; _temp584.tag= Cyc_Absyn_Const_e;
_temp584.f1=( void*) cn; _temp584;}); _temp583;}))); return cn;}