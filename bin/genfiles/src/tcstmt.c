 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern
void* Cyc_Absyn_exn_typ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s); struct Cyc_Stdio___sFILE;
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
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_arr*, struct Cyc_Absyn_Stmt**);
struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple4*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern int* Cyc_Tcutil_new_tvar_id();
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_arr msg_part); struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt);
extern void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p); extern void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct Cyc_CfFlowInfo_Place;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_CfFlowInfo_TupleF= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
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
roots; int in_try; void* tryflow; } ; extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
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
te2,( struct Cyc_Absyn_Exp*) _check_null( where_opt), _tag_arr("switch clause guard",
sizeof( unsigned char), 20u));} if( vs != 0){ te2= Cyc_Tcenv_set_encloser( te2,
s);} Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs != 0){ Cyc_NewControlFlow_set_encloser(
s, Cyc_Tcenv_get_encloser( te));}}} void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{
void* _temp5=( void*) s0->r; struct Cyc_Absyn_Exp* _temp47; struct Cyc_Absyn_Stmt*
_temp49; struct Cyc_Absyn_Stmt* _temp51; struct Cyc_Absyn_Exp* _temp53; struct
Cyc_Absyn_Stmt* _temp55; struct Cyc_Absyn_Stmt* _temp57; struct Cyc_Absyn_Exp*
_temp59; struct Cyc_Absyn_Stmt* _temp61; struct _tuple2 _temp63; struct Cyc_Absyn_Stmt*
_temp65; struct Cyc_Absyn_Exp* _temp67; struct Cyc_Absyn_Stmt* _temp69; struct
_tuple2 _temp71; struct Cyc_Absyn_Stmt* _temp73; struct Cyc_Absyn_Exp* _temp75;
struct _tuple2 _temp77; struct Cyc_Absyn_Stmt* _temp79; struct Cyc_Absyn_Exp*
_temp81; struct Cyc_Absyn_Exp* _temp83; struct _tuple2 _temp85; struct Cyc_Absyn_Stmt*
_temp87; struct Cyc_Absyn_Exp* _temp89; struct Cyc_Absyn_Stmt* _temp91; struct
Cyc_Absyn_Stmt* _temp93; struct Cyc_Absyn_Stmt** _temp95; struct Cyc_Absyn_Stmt*
_temp96; struct Cyc_Absyn_Stmt** _temp98; struct Cyc_Absyn_Stmt* _temp99; struct
Cyc_Absyn_Stmt** _temp101; struct _tagged_arr* _temp102; struct Cyc_Absyn_Switch_clause**
_temp104; struct Cyc_Absyn_Switch_clause*** _temp106; struct Cyc_List_List*
_temp107; struct Cyc_Absyn_Stmt* _temp109; struct _tagged_arr* _temp111; struct
Cyc_List_List* _temp113; struct Cyc_Absyn_Exp* _temp115; struct Cyc_List_List*
_temp117; struct Cyc_Absyn_Exp* _temp119; struct Cyc_List_List* _temp121; struct
Cyc_Absyn_Stmt* _temp123; struct Cyc_Absyn_Stmt* _temp125; struct Cyc_Absyn_Decl*
_temp127; struct Cyc_Absyn_Stmt* _temp129; struct Cyc_Absyn_Vardecl* _temp131;
struct Cyc_Absyn_Tvar* _temp133; struct Cyc_Absyn_Stmt* _temp135; struct Cyc_Absyn_Stmt*
_temp137; _LL7: if( _temp5 ==( void*) Cyc_Absyn_Skip_s){ goto _LL8;} else{ goto
_LL9;} _LL9: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Exp_s:
0){ _LL48: _temp47=(( struct Cyc_Absyn_Exp_s_struct*) _temp5)->f1; goto _LL10;}
else{ goto _LL11;} _LL11: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) ==
Cyc_Absyn_Seq_s: 0){ _LL52: _temp51=(( struct Cyc_Absyn_Seq_s_struct*) _temp5)->f1;
goto _LL50; _LL50: _temp49=(( struct Cyc_Absyn_Seq_s_struct*) _temp5)->f2; goto
_LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp5 > 1u?*(( int*)
_temp5) == Cyc_Absyn_Return_s: 0){ _LL54: _temp53=(( struct Cyc_Absyn_Return_s_struct*)
_temp5)->f1; goto _LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_IfThenElse_s: 0){ _LL60: _temp59=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp5)->f1; goto _LL58; _LL58: _temp57=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp5)->f2; goto _LL56; _LL56: _temp55=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp5)->f3; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_While_s: 0){ _LL64: _temp63=(( struct Cyc_Absyn_While_s_struct*)
_temp5)->f1; _LL68: _temp67= _temp63.f1; goto _LL66; _LL66: _temp65= _temp63.f2;
goto _LL62; _LL62: _temp61=(( struct Cyc_Absyn_While_s_struct*) _temp5)->f2;
goto _LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp5 > 1u?*(( int*)
_temp5) == Cyc_Absyn_For_s: 0){ _LL84: _temp83=(( struct Cyc_Absyn_For_s_struct*)
_temp5)->f1; goto _LL78; _LL78: _temp77=(( struct Cyc_Absyn_For_s_struct*)
_temp5)->f2; _LL82: _temp81= _temp77.f1; goto _LL80; _LL80: _temp79= _temp77.f2;
goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_For_s_struct*) _temp5)->f3; _LL76:
_temp75= _temp71.f1; goto _LL74; _LL74: _temp73= _temp71.f2; goto _LL70; _LL70:
_temp69=(( struct Cyc_Absyn_For_s_struct*) _temp5)->f4; goto _LL20;} else{ goto
_LL21;} _LL21: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Do_s:
0){ _LL92: _temp91=(( struct Cyc_Absyn_Do_s_struct*) _temp5)->f1; goto _LL86;
_LL86: _temp85=(( struct Cyc_Absyn_Do_s_struct*) _temp5)->f2; _LL90: _temp89=
_temp85.f1; goto _LL88; _LL88: _temp87= _temp85.f2; goto _LL22;} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Break_s:
0){ _LL94: _temp93=(( struct Cyc_Absyn_Break_s_struct*) _temp5)->f1; _temp95=&((
struct Cyc_Absyn_Break_s_struct*) _temp5)->f1; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Continue_s:
0){ _LL97: _temp96=(( struct Cyc_Absyn_Continue_s_struct*) _temp5)->f1; _temp98=&((
struct Cyc_Absyn_Continue_s_struct*) _temp5)->f1; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Goto_s: 0){
_LL103: _temp102=(( struct Cyc_Absyn_Goto_s_struct*) _temp5)->f1; goto _LL100;
_LL100: _temp99=(( struct Cyc_Absyn_Goto_s_struct*) _temp5)->f2; _temp101=&((
struct Cyc_Absyn_Goto_s_struct*) _temp5)->f2; goto _LL28;} else{ goto _LL29;}
_LL29: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) == Cyc_Absyn_Fallthru_s:
0){ _LL108: _temp107=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp5)->f1; goto
_LL105; _LL105: _temp104=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp5)->f2;
_temp106=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp5)->f2; goto _LL30;}
else{ goto _LL31;} _LL31: if(( unsigned int) _temp5 > 1u?*(( int*) _temp5) ==
Cyc_Absyn_Label_s: 0){ _LL112: _temp111=(( struct Cyc_Absyn_Label_s_struct*)
_temp5)->f1; goto _LL110; _LL110: _temp109=(( struct Cyc_Absyn_Label_s_struct*)
_temp5)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_Switch_s: 0){ _LL116: _temp115=(( struct Cyc_Absyn_Switch_s_struct*)
_temp5)->f1; goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_Switch_s_struct*)
_temp5)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_SwitchC_s: 0){ _LL120: _temp119=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp5)->f1; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp5)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_TryCatch_s: 0){ _LL124: _temp123=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp5)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp5)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_Decl_s: 0){ _LL128: _temp127=(( struct Cyc_Absyn_Decl_s_struct*)
_temp5)->f1; goto _LL126; _LL126: _temp125=(( struct Cyc_Absyn_Decl_s_struct*)
_temp5)->f2; goto _LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_Region_s: 0){ _LL134: _temp133=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f1; goto _LL132; _LL132: _temp131=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f2; goto _LL130; _LL130: _temp129=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f3; goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_Cut_s: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Cut_s_struct*)
_temp5)->f1; goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp5 >
1u?*(( int*) _temp5) == Cyc_Absyn_Splice_s: 0){ _LL138: _temp137=(( struct Cyc_Absyn_Splice_s_struct*)
_temp5)->f1; goto _LL46;} else{ goto _LL6;} _LL8: return; _LL10: Cyc_Tcexp_tcExp(
te, 0, _temp47); return; _LL12: { struct Cyc_Tcenv_Tenv* _temp141= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp139=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp139[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp140; _temp140.tag= Cyc_Tcenv_Stmt_j; _temp140.f1= _temp49; _temp140;});
_temp139;})); goto _LL142; _LL142: Cyc_Tcstmt_tcStmt( _temp141, _temp51, 1);
while( 1) { void* _temp143=( void*) _temp49->r; struct Cyc_Absyn_Stmt* _temp149;
struct Cyc_Absyn_Stmt* _temp151; _LL145: if(( unsigned int) _temp143 > 1u?*((
int*) _temp143) == Cyc_Absyn_Seq_s: 0){ _LL152: _temp151=(( struct Cyc_Absyn_Seq_s_struct*)
_temp143)->f1; goto _LL150; _LL150: _temp149=(( struct Cyc_Absyn_Seq_s_struct*)
_temp143)->f2; goto _LL146;} else{ goto _LL147;} _LL147: goto _LL148; _LL146:
Cyc_Tcstmt_decorate_stmt( te, _temp49); _temp141= Cyc_Tcenv_set_next( te,( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp153=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp153[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp154; _temp154.tag= Cyc_Tcenv_Stmt_j; _temp154.f1= _temp149; _temp154;});
_temp153;})); Cyc_Tcstmt_tcStmt( _temp141, _temp151, 1); _temp49= _temp149;
continue; _LL148: goto seq_end; _LL144:;} seq_end: Cyc_Tcstmt_tcStmt( te,
_temp49, 1); return;} _LL14: { void* t= Cyc_Tcenv_return_typ( te); if( _temp53
== 0){ void* _temp155= Cyc_Tcutil_compress( t); _LL157: if( _temp155 ==( void*)
Cyc_Absyn_VoidType){ goto _LL158;} else{ goto _LL159;} _LL159: goto _LL160;
_LL158: goto _LL156; _LL160: Cyc_Tcutil_terr( s0->loc,( struct _tagged_arr)({
struct _tagged_arr _temp161= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp161.last_plus_one - _temp161.curr, _temp161.curr);})); goto _LL156; _LL156:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _check_null( _temp53);
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,( struct _tagged_arr)({ struct _tagged_arr _temp162=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct _tagged_arr _temp163= Cyc_Absynpp_typ2string( t); xprintf("returns value of type %.*s but requires %.*s",
_temp162.last_plus_one - _temp162.curr, _temp162.curr, _temp163.last_plus_one -
_temp163.curr, _temp163.curr);}));}{ void* _temp164= Cyc_Tcutil_compress( t);
_LL166: if( _temp164 ==( void*) Cyc_Absyn_VoidType){ goto _LL167;} else{ goto
_LL168;} _LL168: goto _LL169; _LL167: Cyc_Tcutil_terr( s0->loc, _tag_arr("function returns void expression",
sizeof( unsigned char), 33u)); goto _LL165; _LL169: goto _LL165; _LL165:;}}
return;} _LL16: Cyc_Tcexp_tcTest( te, _temp59, _tag_arr("if statement", sizeof(
unsigned char), 13u)); Cyc_Tcstmt_tcStmt( te, _temp57, 1); Cyc_Tcstmt_tcStmt( te,
_temp55, 1); return; _LL18: Cyc_Tcstmt_decorate_stmt( te, _temp65); Cyc_Tcexp_tcTest(
te, _temp67, _tag_arr("while loop", sizeof( unsigned char), 11u)); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp65), _temp61, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt(
te, _temp79); Cyc_Tcstmt_decorate_stmt( te, _temp73); Cyc_Tcexp_tcExp( te, 0,
_temp83); Cyc_Tcexp_tcTest( te, _temp81, _tag_arr("for loop", sizeof(
unsigned char), 9u)); te= Cyc_Tcenv_set_in_loop( te, _temp73); Cyc_Tcstmt_tcStmt(
te, _temp69, 1); Cyc_Tcexp_tcExp( te, 0, _temp75); return; _LL22: Cyc_Tcstmt_decorate_stmt(
te, _temp87); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop( te, _temp87), _temp91, 1);
Cyc_Tcexp_tcTest( te, _temp89, _tag_arr("do loop", sizeof( unsigned char), 8u));
return; _LL24: Cyc_Tcenv_process_break( te, s0, _temp95); return; _LL26: Cyc_Tcenv_process_continue(
te, s0, _temp98); return; _LL28: Cyc_Tcenv_process_goto( te, s0, _temp102,
_temp101); return; _LL30: { struct _tuple3* _temp170= Cyc_Tcenv_process_fallthru(
te, s0, _temp106); goto _LL171; _LL171: if( _temp170 == 0){ Cyc_Tcutil_terr( s0->loc,
_tag_arr("fallthru not in a non-last case", sizeof( unsigned char), 32u));
return;}{ struct _tuple3 _temp174; struct Cyc_List_List* _temp175; struct Cyc_List_List*
_temp177; struct Cyc_Absyn_Switch_clause* _temp179; struct _tuple3* _temp172=(
struct _tuple3*) _check_null( _temp170); _temp174=* _temp172; _LL180: _temp179=
_temp174.f1; goto _LL178; _LL178: _temp177= _temp174.f2; goto _LL176; _LL176:
_temp175= _temp174.f3; goto _LL173; _LL173: { struct Cyc_List_List*
instantiation=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcutil_make_inst_var, Cyc_Tcenv_lookup_type_vars( te),
_temp177); _temp175=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*,
void*), struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcutil_substitute, instantiation, _temp175); for( 0; _temp175 != 0? _temp107
!= 0: 0;( _temp175=(( struct Cyc_List_List*) _check_null( _temp175))->tl,
_temp107=(( struct Cyc_List_List*) _check_null( _temp107))->tl)){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp107))->hd);
if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp107))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp175))->hd)){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp181= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp107))->hd)->topt))->v); struct _tagged_arr _temp182= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp175))->hd); xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp181.last_plus_one - _temp181.curr, _temp181.curr, _temp182.last_plus_one -
_temp182.curr, _temp182.curr);}));}} if( _temp107 != 0){ Cyc_Tcutil_terr( s0->loc,
_tag_arr("too many arguments to explicit fallthru", sizeof( unsigned char), 40u));}
if( _temp175 != 0){ Cyc_Tcutil_terr( s0->loc, _tag_arr("too few arguments to explicit fallthru",
sizeof( unsigned char), 39u));} return;}}} _LL32: Cyc_Tcstmt_tcStmt( Cyc_Tcenv_new_named_block(
s0->loc, Cyc_Tcenv_add_label( te, _temp111, _temp109),({ struct Cyc_Absyn_Tvar*
_temp183=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp183->name=({ struct _tagged_arr* _temp184=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp184[ 0]=( struct _tagged_arr)({ struct
_tagged_arr _temp185=* _temp111; xprintf("`%.*s", _temp185.last_plus_one -
_temp185.curr, _temp185.curr);}); _temp184;}); _temp183->identity= Cyc_Tcutil_new_tvar_id();
_temp183->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp183;})), _temp109, 0); return; _LL34: Cyc_Tcexp_tcExp(
te, 0, _temp115);{ void* _temp186=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp115->topt))->v; goto _LL187; _LL187: te= Cyc_Tcenv_set_in_switch( te); te=
Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp113); for( 0; scs != 0; scs=((
struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Pat* _temp188=((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->pattern;
goto _LL189; _LL189: { struct Cyc_List_List* _temp192; struct Cyc_List_List*
_temp194; struct _tuple5 _temp190= Cyc_Tcpat_tcPat( te, _temp188,( void**)&
_temp186, 0); _LL195: _temp194= _temp190.f1; goto _LL193; _LL193: _temp192=
_temp190.f2; goto _LL191; _LL191: if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp188->topt))->v, _temp186)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc,( struct _tagged_arr)({
struct _tagged_arr _temp196= Cyc_Absynpp_typ2string( _temp186); struct
_tagged_arr _temp197= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp188->topt))->v); xprintf("switch on type %.*s, but case expects type %.*s",
_temp196.last_plus_one - _temp196.curr, _temp196.curr, _temp197.last_plus_one -
_temp197.curr, _temp197.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp188);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp198=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp198->v=( void*) _temp192;
_temp198;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc, te, _temp194, _temp192,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp194, _temp192,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd);}}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp113); return;} _LL36: { struct Cyc_List_List* _temp199= 0; goto
_LL200; _LL200: for( 0; _temp117 != 0; _temp117=(( struct Cyc_List_List*)
_check_null( _temp117))->tl){ struct Cyc_Absyn_SwitchC_clause _temp203; struct
Cyc_Position_Segment* _temp204; struct Cyc_Absyn_Stmt* _temp206; struct Cyc_Absyn_Exp*
_temp208; struct Cyc_Absyn_SwitchC_clause* _temp201=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp117))->hd; _temp203=* _temp201; _LL209:
_temp208= _temp203.cnst_exp; goto _LL207; _LL207: _temp206= _temp203.body; goto
_LL205; _LL205: _temp204= _temp203.loc; goto _LL202; _LL202: { struct Cyc_Absyn_Pat*
pat; if( _temp208 != 0){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp208)); pat= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp210=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp210[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp211; _temp211.tag= Cyc_Absyn_Int_p;
_temp211.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp211.f2=( int) Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp208)); _temp211;}); _temp210;}),((
struct Cyc_Absyn_Exp*) _check_null( _temp208))->loc);} else{ pat= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, _temp204);} _temp199=({ struct Cyc_List_List* _temp212=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp212->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp213=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp213->pattern= pat;
_temp213->pat_vars= 0; _temp213->where_clause= 0; _temp213->body= _temp206;
_temp213->loc= _temp204; _temp213;}); _temp212->tl= _temp199; _temp212;});}}(
void*)( s0->r=( void*)(( void*)( Cyc_Absyn_switch_stmt( _temp119,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp199), 0))->r)); Cyc_Tcstmt_tcStmt(
te, s0, new_block); return;} _LL38:( void*)( _temp123->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt(( void*) _temp123->r, _temp123->loc), Cyc_Absyn_skip_stmt(
_temp123->loc), _temp123->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try( te), s0), _temp123, 1); te= Cyc_Tcenv_set_in_switch( te);
te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* _temp214=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp121); goto _LL215; _LL215: for( 0;
_temp214 != 0; _temp214=(( struct Cyc_List_List*) _check_null( _temp214))->tl){
struct Cyc_Absyn_Pat* _temp216=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp214))->hd)->pattern; goto _LL217; _LL217: { struct Cyc_List_List*
_temp220; struct Cyc_List_List* _temp222; struct _tuple5 _temp218= Cyc_Tcpat_tcPat(
te, _temp216,( void**)& Cyc_Absyn_exn_typ, 0); _LL223: _temp222= _temp218.f1;
goto _LL221; _LL221: _temp220= _temp218.f2; goto _LL219; _LL219: if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp216->topt))->v, Cyc_Absyn_exn_typ)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp214))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp224= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp216->topt))->v); xprintf("expected xtunion exn but found %.*s", _temp224.last_plus_one
- _temp224.curr, _temp224.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp216);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp214))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp225=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp225->v=( void*)
_temp220; _temp225;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp214))->hd)->loc, te, _temp222, _temp220,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp214))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp214))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp222, _temp220,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp214))->hd);}}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp121); return; _LL40: { struct _tagged_arr unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp226=( void*)
_temp127->r; struct Cyc_Absyn_Vardecl* _temp252; int _temp254; int* _temp256;
struct Cyc_Absyn_Exp* _temp257; struct Cyc_Core_Opt* _temp259; struct Cyc_Core_Opt**
_temp261; struct Cyc_Core_Opt* _temp262; struct Cyc_Core_Opt** _temp264; struct
Cyc_Absyn_Pat* _temp265; struct Cyc_List_List* _temp267; struct Cyc_List_List*
_temp269; struct _tagged_arr* _temp271; struct Cyc_List_List* _temp273; struct
_tuple0* _temp275; struct Cyc_Absyn_Fndecl* _temp277; struct Cyc_Absyn_Structdecl*
_temp279; struct Cyc_Absyn_Uniondecl* _temp281; struct Cyc_Absyn_Tuniondecl*
_temp283; struct Cyc_Absyn_Enumdecl* _temp285; struct Cyc_Absyn_Typedefdecl*
_temp287; struct Cyc_List_List* _temp289; _LL228: if(*(( int*) _temp226) == Cyc_Absyn_Var_d){
_LL253: _temp252=(( struct Cyc_Absyn_Var_d_struct*) _temp226)->f1; goto _LL229;}
else{ goto _LL230;} _LL230: if(*(( int*) _temp226) == Cyc_Absyn_Let_d){ _LL266:
_temp265=(( struct Cyc_Absyn_Let_d_struct*) _temp226)->f1; goto _LL263; _LL263:
_temp262=(( struct Cyc_Absyn_Let_d_struct*) _temp226)->f2; _temp264=&(( struct
Cyc_Absyn_Let_d_struct*) _temp226)->f2; goto _LL260; _LL260: _temp259=(( struct
Cyc_Absyn_Let_d_struct*) _temp226)->f3; _temp261=&(( struct Cyc_Absyn_Let_d_struct*)
_temp226)->f3; goto _LL258; _LL258: _temp257=(( struct Cyc_Absyn_Let_d_struct*)
_temp226)->f4; goto _LL255; _LL255: _temp254=(( struct Cyc_Absyn_Let_d_struct*)
_temp226)->f5; _temp256=&(( struct Cyc_Absyn_Let_d_struct*) _temp226)->f5; goto
_LL231;} else{ goto _LL232;} _LL232: if(*(( int*) _temp226) == Cyc_Absyn_Letv_d){
_LL268: _temp267=(( struct Cyc_Absyn_Letv_d_struct*) _temp226)->f1; goto _LL233;}
else{ goto _LL234;} _LL234: if(*(( int*) _temp226) == Cyc_Absyn_Namespace_d){
_LL272: _temp271=(( struct Cyc_Absyn_Namespace_d_struct*) _temp226)->f1; goto
_LL270; _LL270: _temp269=(( struct Cyc_Absyn_Namespace_d_struct*) _temp226)->f2;
goto _LL235;} else{ goto _LL236;} _LL236: if(*(( int*) _temp226) == Cyc_Absyn_Using_d){
_LL276: _temp275=(( struct Cyc_Absyn_Using_d_struct*) _temp226)->f1; goto _LL274;
_LL274: _temp273=(( struct Cyc_Absyn_Using_d_struct*) _temp226)->f2; goto _LL237;}
else{ goto _LL238;} _LL238: if(*(( int*) _temp226) == Cyc_Absyn_Fn_d){ _LL278:
_temp277=(( struct Cyc_Absyn_Fn_d_struct*) _temp226)->f1; goto _LL239;} else{
goto _LL240;} _LL240: if(*(( int*) _temp226) == Cyc_Absyn_Struct_d){ _LL280:
_temp279=(( struct Cyc_Absyn_Struct_d_struct*) _temp226)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(*(( int*) _temp226) == Cyc_Absyn_Union_d){ _LL282:
_temp281=(( struct Cyc_Absyn_Union_d_struct*) _temp226)->f1; goto _LL243;} else{
goto _LL244;} _LL244: if(*(( int*) _temp226) == Cyc_Absyn_Tunion_d){ _LL284:
_temp283=(( struct Cyc_Absyn_Tunion_d_struct*) _temp226)->f1; goto _LL245;}
else{ goto _LL246;} _LL246: if(*(( int*) _temp226) == Cyc_Absyn_Enum_d){ _LL286:
_temp285=(( struct Cyc_Absyn_Enum_d_struct*) _temp226)->f1; goto _LL247;} else{
goto _LL248;} _LL248: if(*(( int*) _temp226) == Cyc_Absyn_Typedef_d){ _LL288:
_temp287=(( struct Cyc_Absyn_Typedef_d_struct*) _temp226)->f1; goto _LL249;}
else{ goto _LL250;} _LL250: if(*(( int*) _temp226) == Cyc_Absyn_ExternC_d){
_LL290: _temp289=(( struct Cyc_Absyn_ExternC_d_struct*) _temp226)->f1; goto
_LL251;} else{ goto _LL227;} _LL229: { struct Cyc_Absyn_Vardecl _temp293; struct
Cyc_List_List* _temp294; struct Cyc_Core_Opt* _temp296; struct Cyc_Core_Opt**
_temp298; struct Cyc_Absyn_Exp* _temp299; void* _temp301; struct Cyc_Absyn_Tqual
_temp303; struct _tuple0* _temp305; struct _tuple0 _temp307; struct _tagged_arr*
_temp308; void* _temp310; void* _temp312; struct Cyc_Absyn_Vardecl* _temp291=
_temp252; _temp293=* _temp291; _LL313: _temp312=( void*) _temp293.sc; goto
_LL306; _LL306: _temp305= _temp293.name; _temp307=* _temp305; _LL311: _temp310=
_temp307.f1; goto _LL309; _LL309: _temp308= _temp307.f2; goto _LL304; _LL304:
_temp303= _temp293.tq; goto _LL302; _LL302: _temp301=( void*) _temp293.type;
goto _LL300; _LL300: _temp299= _temp293.initializer; goto _LL297; _LL297:
_temp296= _temp293.rgn; _temp298=&(* _temp291).rgn; goto _LL295; _LL295:
_temp294= _temp293.attributes; goto _LL292; _LL292: { void* _temp314= Cyc_Tcenv_curr_rgn(
te); goto _LL315; _LL315: { int is_local;{ void* _temp316= _temp312; _LL318: if(
_temp316 ==( void*) Cyc_Absyn_Static){ goto _LL319;} else{ goto _LL320;} _LL320:
if( _temp316 ==( void*) Cyc_Absyn_Extern){ goto _LL321;} else{ goto _LL322;}
_LL322: if( _temp316 ==( void*) Cyc_Absyn_ExternC){ goto _LL323;} else{ goto
_LL324;} _LL324: if( _temp316 ==( void*) Cyc_Absyn_Abstract){ goto _LL325;}
else{ goto _LL326;} _LL326: if( _temp316 ==( void*) Cyc_Absyn_Public){ goto
_LL327;} else{ goto _LL317;} _LL319: goto _LL321; _LL321: goto _LL323; _LL323:
is_local= 0; goto _LL317; _LL325: Cyc_Tcutil_terr( _temp127->loc, _tag_arr("bad abstract scope for local variable",
sizeof( unsigned char), 38u)); goto _LL327; _LL327: is_local= 1; goto _LL317;
_LL317:;}* _temp298= is_local?({ struct Cyc_Core_Opt* _temp328=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp328->v=( void*) _temp314;
_temp328;}):({ struct Cyc_Core_Opt* _temp329=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp329->v=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp329;});{ void* _temp330= _temp310; struct Cyc_List_List* _temp340; struct
Cyc_List_List* _temp342; _LL332: if( _temp330 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp330 > 1u?*(( int*)
_temp330) == Cyc_Absyn_Rel_n: 0){ _LL341: _temp340=(( struct Cyc_Absyn_Rel_n_struct*)
_temp330)->f1; if( _temp340 == 0){ goto _LL335;} else{ goto _LL336;}} else{ goto
_LL336;} _LL336: if(( unsigned int) _temp330 > 1u?*(( int*) _temp330) == Cyc_Absyn_Abs_n:
0){ _LL343: _temp342=(( struct Cyc_Absyn_Abs_n_struct*) _temp330)->f1; goto
_LL337;} else{ goto _LL338;} _LL338: goto _LL339; _LL333: goto _LL331; _LL335:(*
_temp252->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL331; _LL337:( int) _throw(((
void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcstmt: Abs_n declaration",
sizeof( unsigned char), 26u))); _LL339: Cyc_Tcutil_terr( _temp127->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u)); goto _LL331; _LL331:;}{ void* _temp344= Cyc_Tcutil_compress(
_temp301); struct Cyc_Absyn_Exp* _temp351; struct Cyc_Absyn_Tqual _temp353; void*
_temp355; _LL346: if(( unsigned int) _temp344 > 4u?*(( int*) _temp344) == Cyc_Absyn_ArrayType:
0){ _LL356: _temp355=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp344)->f1;
goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_ArrayType_struct*) _temp344)->f2;
goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_ArrayType_struct*) _temp344)->f3;
if( _temp351 == 0){ goto _LL350;} else{ goto _LL348;}} else{ goto _LL348;}
_LL350: if( _temp252->initializer != 0){ goto _LL347;} else{ goto _LL348;}
_LL348: goto _LL349; _LL347:{ void* _temp357=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp252->initializer))->r; void* _temp367; struct _tagged_arr
_temp369; struct Cyc_List_List* _temp371; struct Cyc_Core_Opt* _temp373; struct
Cyc_List_List* _temp375; _LL359: if(*(( int*) _temp357) == Cyc_Absyn_Const_e){
_LL368: _temp367=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp357)->f1; if((
unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_String_c: 0){
_LL370: _temp369=(( struct Cyc_Absyn_String_c_struct*) _temp367)->f1; goto
_LL360;} else{ goto _LL361;}} else{ goto _LL361;} _LL361: if(*(( int*) _temp357)
== Cyc_Absyn_UnresolvedMem_e){ _LL374: _temp373=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp357)->f1; goto _LL372; _LL372: _temp371=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp357)->f2; goto _LL362;} else{ goto _LL363;} _LL363: if(*(( int*) _temp357)
== Cyc_Absyn_Array_e){ _LL376: _temp375=(( struct Cyc_Absyn_Array_e_struct*)
_temp357)->f1; goto _LL364;} else{ goto _LL365;} _LL365: goto _LL366; _LL360:
_temp301=( void*)( _temp252->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp377=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp377[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp378; _temp378.tag= Cyc_Absyn_ArrayType;
_temp378.f1=( void*) _temp355; _temp378.f2= _temp353; _temp378.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( _get_arr_size( _temp369, sizeof( unsigned char)), 0);
_temp378;}); _temp377;}))); goto _LL358; _LL362: _temp375= _temp371; goto _LL364;
_LL364: _temp301=( void*)( _temp252->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp379=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp379[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp380; _temp380.tag= Cyc_Absyn_ArrayType;
_temp380.f1=( void*) _temp355; _temp380.f2= _temp353; _temp380.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp375), 0); _temp380;}); _temp379;}))); goto _LL358; _LL366: goto _LL358;
_LL358:;} goto _LL345; _LL349: goto _LL345; _LL345:;}{ struct Cyc_List_List*
_temp381= ! is_local? 0: Cyc_Tcenv_lookup_type_vars( te); goto _LL382; _LL382:
Cyc_Tcutil_check_type( s0->loc, te, _temp381,( void*) Cyc_Absyn_MemKind,
_temp301);{ struct Cyc_Tcenv_Tenv* _temp383= Cyc_Tcenv_add_local_var( _temp125->loc,
te, _temp252); goto _LL384; _LL384: if( _temp312 ==( void*) Cyc_Absyn_Extern? 1:
_temp312 ==( void*) Cyc_Absyn_ExternC){ Cyc_Tcutil_terr( _temp127->loc, _tag_arr("local extern declarations not yet supported",
sizeof( unsigned char), 44u));} if( _temp299 != 0){ Cyc_Tcexp_tcExpInitializer(
_temp383,( void**)& _temp301,( struct Cyc_Absyn_Exp*) _check_null( _temp299));
if( ! Cyc_Tcutil_coerce_assign( _temp383,( struct Cyc_Absyn_Exp*) _check_null(
_temp299), _temp301)){ Cyc_Tcutil_terr( _temp127->loc,( struct _tagged_arr)({
struct _tagged_arr _temp385=* _temp308; struct _tagged_arr _temp386= Cyc_Absynpp_typ2string(
_temp301); struct _tagged_arr _temp387= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp299))->topt))->v);
xprintf("%.*s declared with type %.*s, initialized with type %.*s", _temp385.last_plus_one
- _temp385.curr, _temp385.curr, _temp386.last_plus_one - _temp386.curr, _temp386.curr,
_temp387.last_plus_one - _temp387.curr, _temp387.curr);}));}} Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser( _temp383, s0), _temp125, 0); return;}}}}} _LL231: {
struct Cyc_Core_Opt* _temp388=* _temp261; goto _LL389; _LL389: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp257);* _temp261= _temp257->topt;{ struct Cyc_List_List* _temp392;
struct Cyc_List_List* _temp394; struct _tuple5 _temp390= Cyc_Tcpat_tcPat( te,
_temp265,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null(
_temp257->topt))->v)), 0); _LL395: _temp394= _temp390.f1; goto _LL393; _LL393:
_temp392= _temp390.f2; goto _LL391; _LL391:* _temp264=({ struct Cyc_Core_Opt*
_temp396=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp396->v=( void*) _temp392; _temp396;}); if( _temp388 != 0? ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp265->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp388))->v): 0){ Cyc_Tcutil_terr( _temp127->loc,
_tag_arr("type of pattern does not match declared type", sizeof( unsigned char),
45u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp265->topt))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp257->topt))->v)){
Cyc_Tcutil_terr( _temp127->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp397= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp265->topt))->v); struct _tagged_arr _temp398= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp257->topt))->v); xprintf("pattern type %.*s does not match definition type %.*s",
_temp397.last_plus_one - _temp397.curr, _temp397.curr, _temp398.last_plus_one -
_temp398.curr, _temp398.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp265);}* _temp256= Cyc_Tcpat_check_let_pat_exhaustive( _temp265->loc,
_temp265); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp394, _temp392, _temp125,
0, 0); return;}} _LL233: { void* _temp399= Cyc_Tcenv_curr_rgn( te); goto _LL400;
_LL400: { struct Cyc_Tcenv_Tenv* _temp401= te; goto _LL402; _LL402: for( 0;
_temp267 != 0; _temp267=(( struct Cyc_List_List*) _check_null( _temp267))->tl){
struct Cyc_Absyn_Vardecl* _temp403=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp267))->hd; goto _LL404; _LL404: { struct Cyc_Absyn_Vardecl
_temp407; struct Cyc_List_List* _temp408; struct Cyc_Core_Opt* _temp410; struct
Cyc_Core_Opt** _temp412; struct Cyc_Absyn_Exp* _temp413; void* _temp415; struct
Cyc_Absyn_Tqual _temp417; struct _tuple0* _temp419; struct _tuple0 _temp421;
struct _tagged_arr* _temp422; void* _temp424; void* _temp426; struct Cyc_Absyn_Vardecl*
_temp405= _temp403; _temp407=* _temp405; _LL427: _temp426=( void*) _temp407.sc;
goto _LL420; _LL420: _temp419= _temp407.name; _temp421=* _temp419; _LL425:
_temp424= _temp421.f1; goto _LL423; _LL423: _temp422= _temp421.f2; goto _LL418;
_LL418: _temp417= _temp407.tq; goto _LL416; _LL416: _temp415=( void*) _temp407.type;
goto _LL414; _LL414: _temp413= _temp407.initializer; goto _LL411; _LL411:
_temp410= _temp407.rgn; _temp412=&(* _temp405).rgn; goto _LL409; _LL409:
_temp408= _temp407.attributes; goto _LL406; _LL406:* _temp412=({ struct Cyc_Core_Opt*
_temp428=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp428->v=( void*) _temp399; _temp428;});{ void* _temp429= _temp424; struct
Cyc_List_List* _temp439; struct Cyc_List_List* _temp441; _LL431: if( _temp429 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL432;} else{ goto _LL433;} _LL433: if((
unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Rel_n: 0){ _LL440:
_temp439=(( struct Cyc_Absyn_Rel_n_struct*) _temp429)->f1; if( _temp439 == 0){
goto _LL434;} else{ goto _LL435;}} else{ goto _LL435;} _LL435: if(( unsigned int)
_temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Abs_n: 0){ _LL442: _temp441=((
struct Cyc_Absyn_Abs_n_struct*) _temp429)->f1; goto _LL436;} else{ goto _LL437;}
_LL437: goto _LL438; _LL432: goto _LL430; _LL434:(* _temp403->name).f1=( void*)
Cyc_Absyn_Loc_n; goto _LL430; _LL436:( int) _throw((( void*(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcstmt: Abs_n declaration", sizeof(
unsigned char), 26u))); _LL438: Cyc_Tcutil_terr( _temp127->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u)); goto _LL430; _LL430:;} Cyc_Tcutil_check_type( s0->loc,
_temp401, Cyc_Tcenv_lookup_type_vars( _temp401),( void*) Cyc_Absyn_MemKind,
_temp415); _temp401= Cyc_Tcenv_add_local_var( _temp125->loc, _temp401, _temp403);}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp401, s0), _temp125, 0); return;}}
_LL235: unimp_msg_part= _tag_arr("namespace", sizeof( unsigned char), 10u); goto
_LL227; _LL237: unimp_msg_part= _tag_arr("using", sizeof( unsigned char), 6u);
goto _LL227; _LL239: unimp_msg_part= _tag_arr("function", sizeof( unsigned char),
9u); goto _LL227; _LL241: unimp_msg_part= _tag_arr("struct", sizeof(
unsigned char), 7u); goto _LL227; _LL243: unimp_msg_part= _tag_arr("union",
sizeof( unsigned char), 6u); goto _LL227; _LL245: unimp_msg_part= _tag_arr("[x]tunion",
sizeof( unsigned char), 10u); goto _LL227; _LL247: unimp_msg_part= _tag_arr("enum",
sizeof( unsigned char), 5u); goto _LL227; _LL249: unimp_msg_part= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL227; _LL251: unimp_msg_part= _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u); goto _LL227; _LL227:;}( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp443= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp443.last_plus_one - _temp443.curr, _temp443.curr);})));} _LL42:( void*)(
_temp129->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp129->r, _temp129->loc), Cyc_Absyn_skip_stmt( _temp129->loc), _temp129->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp446; struct Cyc_List_List* _temp447; struct Cyc_Core_Opt* _temp449; struct
Cyc_Core_Opt** _temp451; struct Cyc_Absyn_Exp* _temp452; void* _temp454; struct
Cyc_Absyn_Tqual _temp456; struct _tuple0* _temp458; void* _temp460; struct Cyc_Absyn_Vardecl*
_temp444= _temp131; _temp446=* _temp444; _LL461: _temp460=( void*) _temp446.sc;
goto _LL459; _LL459: _temp458= _temp446.name; goto _LL457; _LL457: _temp456=
_temp446.tq; goto _LL455; _LL455: _temp454=( void*) _temp446.type; goto _LL453;
_LL453: _temp452= _temp446.initializer; goto _LL450; _LL450: _temp449= _temp446.rgn;
_temp451=&(* _temp444).rgn; goto _LL448; _LL448: _temp447= _temp446.attributes;
goto _LL445; _LL445: { void* _temp462= Cyc_Tcenv_curr_rgn( te); goto _LL463;
_LL463: { void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct* _temp468=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp468[ 0]=({ struct Cyc_Absyn_VarType_struct _temp469; _temp469.tag= Cyc_Absyn_VarType;
_temp469.f1= _temp133; _temp469;}); _temp468;});* _temp451=({ struct Cyc_Core_Opt*
_temp464=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp464->v=( void*) _temp462; _temp464;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp465=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp465->hd=( void*) _temp133; _temp465->tl= 0;
_temp465;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp454); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp454,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp466=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp466[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp467; _temp467.tag=
Cyc_Absyn_RgnHandleType; _temp467.f1=( void*) rgn_typ; _temp467;}); _temp466;}))){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("region stmt: type of region handle is wrong!",
sizeof( unsigned char), 45u));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_add_local_var(
_temp129->loc, Cyc_Tcenv_enter_try( te), _temp131), s0), _temp129, 0); return;}}}
_LL44:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcStmt: cut is not implemented", sizeof( unsigned char), 31u))); _LL46:(
int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcStmt: splice is not implemented",
sizeof( unsigned char), 34u))); _LL6:;}}