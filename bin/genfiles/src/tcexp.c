 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; struct
_tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*);
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct
Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void*
Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn); extern void*
Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_zstrcmp(
struct _tagged_arr, struct _tagged_arr); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po); extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void* Cyc_Tcutil_impos( struct _tagged_arr); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple4*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); extern
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int
new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*); extern
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; static void* Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp0;}));} static struct Cyc_Core_Opt
Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt Cyc_Tcexp_ak={(
void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*)
2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static
void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp1; _push_handler(& _temp1);{
int _temp3= 0; if( setjmp( _temp1.handler)){ _temp3= 1;} if( ! _temp3){{ void*
_temp4= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void* _temp14; struct Cyc_Absyn_Enumfield*
_temp16; struct Cyc_Absyn_Enumdecl* _temp18; struct Cyc_Absyn_Tunionfield*
_temp20; struct Cyc_Absyn_Tuniondecl* _temp22; struct Cyc_Absyn_Structdecl*
_temp24; _LL6: if(*(( int*) _temp4) == Cyc_Tcenv_VarRes){ _LL15: _temp14=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp4)->f1; goto _LL7;} else{ goto _LL8;} _LL8:
if(*(( int*) _temp4) == Cyc_Tcenv_EnumRes){ _LL19: _temp18=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp4)->f1; goto _LL17; _LL17: _temp16=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp4)->f2; goto _LL9;} else{ goto _LL10;} _LL10: if(*(( int*) _temp4) == Cyc_Tcenv_TunionRes){
_LL23: _temp22=(( struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f1; goto _LL21;
_LL21: _temp20=(( struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f2; goto _LL11;}
else{ goto _LL12;} _LL12: if(*(( int*) _temp4) == Cyc_Tcenv_StructRes){ _LL25:
_temp24=(( struct Cyc_Tcenv_StructRes_struct*) _temp4)->f1; goto _LL13;} else{
goto _LL5;} _LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp26=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp27; _temp27.tag= Cyc_Absyn_Var_e;
_temp27.f1= q; _temp27.f2=( void*) _temp14; _temp27;}); _temp26;}))); goto _LL5;
_LL9:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp28=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp29; _temp29.tag= Cyc_Absyn_Enum_e;
_temp29.f1= q; _temp29.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp29.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp29;}); _temp28;}))); goto _LL5; _LL11:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp30=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp31; _temp31.tag= Cyc_Absyn_Tunion_e;
_temp31.f1= 0; _temp31.f2= 0; _temp31.f3= 0; _temp31.f4= _temp22; _temp31.f5=
_temp20; _temp31;}); _temp30;}))); goto _LL5; _LL13: Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp32= Cyc_Absynpp_qvar2string( q);
xprintf("bad occurrence of struct name %.*s", _get_arr_size( _temp32, 1u),
_temp32.curr);}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp33=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp34; _temp34.tag= Cyc_Absyn_Var_e;
_temp34.f1= q; _temp34.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp34;});
_temp33;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp36= _temp2; _LL38: if( _temp36 == Cyc_Dict_Absent){ goto
_LL39;} else{ goto _LL40;} _LL40: goto _LL41; _LL39:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp42=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp43; _temp43.tag= Cyc_Absyn_Var_e; _temp43.f1= q; _temp43.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp43;}); _temp42;}))); goto _LL37; _LL41:( void)
_throw( _temp36); _LL37:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp44=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp44->f1= 0; _temp44->f2= e; _temp44;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp45=( void*)
e1->r; struct _tuple1* _temp51; _LL47: if(*(( int*) _temp45) == Cyc_Absyn_UnknownId_e){
_LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp45)->f1; goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct _handler_cons _temp53;
_push_handler(& _temp53);{ int _temp55= 0; if( setjmp( _temp53.handler)){
_temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68; struct Cyc_Absyn_Tuniondecl*
_temp70; struct Cyc_Absyn_Structdecl* _temp72; struct Cyc_Absyn_Enumfield*
_temp74; struct Cyc_Absyn_Enumdecl* _temp76; _LL58: if(*(( int*) _temp56) == Cyc_Tcenv_VarRes){
_LL67: _temp66=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto
_LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp56) == Cyc_Tcenv_TunionRes){
_LL71: _temp70=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69;
_LL69: _temp68=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;}
else{ goto _LL62;} _LL62: if(*(( int*) _temp56) == Cyc_Tcenv_StructRes){ _LL73:
_temp72=(( struct Cyc_Tcenv_StructRes_struct*) _temp56)->f1; goto _LL63;} else{
goto _LL64;} _LL64: if(*(( int*) _temp56) == Cyc_Tcenv_EnumRes){ _LL77: _temp76=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f1; goto _LL75; _LL75: _temp74=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f2; goto _LL65;} else{ goto _LL57;}
_LL59:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp78=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp78[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp79; _temp79.tag= Cyc_Absyn_FnCall_e;
_temp79.f1= e1; _temp79.f2= es; _temp79.f3= 0; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _LL61: if( _temp68->typs == 0){ Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp80= Cyc_Absynpp_qvar2string(
_temp68->name); xprintf("%.*s is a constant, not a function", _get_arr_size(
_temp80, 1u), _temp80.curr);}));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp81=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp82; _temp82.tag= Cyc_Absyn_Tunion_e;
_temp82.f1= 0; _temp82.f2= 0; _temp82.f3= es; _temp82.f4= _temp70; _temp82.f5=
_temp68; _temp82;}); _temp81;}))); _npop_handler( 0u); return; _npop_handler( 0u);
return; _LL63: { struct Cyc_List_List* _temp83=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcexp_make_struct_arg, es); goto _LL84; _LL84: if( _temp72->name != 0){(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp85=(
struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp85[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp86; _temp86.tag= Cyc_Absyn_Struct_e;
_temp86.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp86.f2= 0; _temp86.f3= _temp83; _temp86.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp86;}); _temp85;})));} else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name",
sizeof( unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp87=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp87[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp88; _temp88.tag= Cyc_Absyn_Struct_e;
_temp88.f1= _temp51; _temp88.f2= 0; _temp88.f3= _temp83; _temp88.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp88;}); _temp87;})));} _npop_handler( 0u); return;} _LL65: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp89= Cyc_Absynpp_qvar2string(
_temp51); xprintf("%.*s is an enum constructor, not a function", _get_arr_size(
_temp89, 1u), _temp89.curr);})); _npop_handler( 0u); return; _LL57:;};
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp91=
_temp54; _LL93: if( _temp91 == Cyc_Dict_Absent){ goto _LL94;} else{ goto _LL95;}
_LL95: goto _LL96; _LL94: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({
struct _tagged_arr _temp97= Cyc_Absynpp_qvar2string( _temp51); xprintf("unbound identifier %.*s",
_get_arr_size( _temp97, 1u), _temp97.curr);}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp98=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp98[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp99; _temp99.tag= Cyc_Absyn_FnCall_e; _temp99.f1= e1; _temp99.f2= es;
_temp99.f3= 0; _temp99;}); _temp98;}))); return; _LL96:( void) _throw( _temp91);
_LL92:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp100=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp100[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp101; _temp101.tag= Cyc_Absyn_FnCall_e;
_temp101.f1= e1; _temp101.f2= es; _temp101.f3= 0; _temp101;}); _temp100;})));
return; _LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp102=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp102[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp103; _temp103.tag= Cyc_Absyn_Array_e; _temp103.f1= des; _temp103;});
_temp102;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp104=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp114; struct Cyc_Absyn_Tqual
_temp116; void* _temp118; struct Cyc_Absyn_Structdecl** _temp120; struct Cyc_List_List*
_temp122; struct _tuple1* _temp124; struct Cyc_List_List* _temp126; _LL106: if((
unsigned int) _temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_ArrayType: 0){
_LL119: _temp118=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp104)->f1;
goto _LL117; _LL117: _temp116=(( struct Cyc_Absyn_ArrayType_struct*) _temp104)->f2;
goto _LL115; _LL115: _temp114=(( struct Cyc_Absyn_ArrayType_struct*) _temp104)->f3;
goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int) _temp104 > 4u?*((
int*) _temp104) == Cyc_Absyn_StructType: 0){ _LL125: _temp124=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f1; goto _LL123; _LL123: _temp122=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f2; goto _LL121; _LL121: _temp120=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f3; goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int)
_temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_AnonStructType: 0){ _LL127:
_temp126=(( struct Cyc_Absyn_AnonStructType_struct*) _temp104)->f1; goto _LL111;}
else{ goto _LL112;} _LL112: goto _LL113; _LL107:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp128=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp128[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp129; _temp129.tag= Cyc_Absyn_Array_e; _temp129.f1= des; _temp129;});
_temp128;}))); goto _LL105; _LL109: if( _temp120 == 0? 1: _temp124 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("Compiler bug: struct type not properly set", sizeof(
unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp130=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp131; _temp131.tag= Cyc_Absyn_Struct_e;
_temp131.f1=( struct _tuple1*) _check_null( _temp124); _temp131.f2= 0; _temp131.f3=
des; _temp131.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp120)); _temp131;}); _temp130;}))); goto _LL105; _LL111:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp132=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp132[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp133; _temp133.tag= Cyc_Absyn_AnonStruct_e;
_temp133.f1=( void*) t; _temp133.f2= des; _temp133;}); _temp132;}))); goto
_LL105; _LL113:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp134=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp134[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp135; _temp135.tag= Cyc_Absyn_Array_e;
_temp135.f1= des; _temp135;}); _temp134;}))); goto _LL105; _LL105:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp136= msg_part; struct _tagged_arr _temp137= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_get_arr_size( _temp136, 1u), _temp136.curr, _get_arr_size( _temp137, 1u),
_temp137.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){
void* t;{ void* _temp138= c; unsigned char _temp154; void* _temp156; short
_temp158; void* _temp160; long long _temp162; void* _temp164; struct _tagged_arr
_temp166; int _temp168; void* _temp170; struct _tagged_arr _temp172; _LL140: if((
unsigned int) _temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Char_c: 0){ _LL157:
_temp156=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp138)->f1; goto _LL155;
_LL155: _temp154=(( struct Cyc_Absyn_Char_c_struct*) _temp138)->f2; goto _LL141;}
else{ goto _LL142;} _LL142: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138)
== Cyc_Absyn_Short_c: 0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp138)->f1; goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_Short_c_struct*)
_temp138)->f2; goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int)
_temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_LongLong_c: 0){ _LL165: _temp164=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp138)->f1; goto _LL163; _LL163:
_temp162=(( struct Cyc_Absyn_LongLong_c_struct*) _temp138)->f2; goto _LL145;}
else{ goto _LL146;} _LL146: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138)
== Cyc_Absyn_Float_c: 0){ _LL167: _temp166=(( struct Cyc_Absyn_Float_c_struct*)
_temp138)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Int_c: 0){ _LL171: _temp170=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp138)->f1; goto _LL169; _LL169:
_temp168=(( struct Cyc_Absyn_Int_c_struct*) _temp138)->f2; goto _LL149;} else{
goto _LL150;} _LL150: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138) ==
Cyc_Absyn_String_c: 0){ _LL173: _temp172=(( struct Cyc_Absyn_String_c_struct*)
_temp138)->f1; goto _LL151;} else{ goto _LL152;} _LL152: if( _temp138 ==( void*)
Cyc_Absyn_Null_c){ goto _LL153;} else{ goto _LL139;} _LL141: t= _temp156 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL139;
_LL143: t= _temp160 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL139; _LL145: t= _temp164 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL139; _LL147: t= Cyc_Absyn_float_t; goto _LL139;
_LL149: if( topt == 0){ t= _temp170 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp174= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp184; void* _temp186; void* _temp188; void*
_temp190; void* _temp192; void* _temp194; _LL176: if(( unsigned int) _temp174 >
4u?*(( int*) _temp174) == Cyc_Absyn_IntType: 0){ _LL187: _temp186=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp174)->f1; goto _LL185; _LL185: _temp184=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp174)->f2; if( _temp184 ==( void*)
Cyc_Absyn_B1){ goto _LL177;} else{ goto _LL178;}} else{ goto _LL178;} _LL178:
if(( unsigned int) _temp174 > 4u?*(( int*) _temp174) == Cyc_Absyn_IntType: 0){
_LL191: _temp190=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp174)->f1;
goto _LL189; _LL189: _temp188=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp174)->f2; if( _temp188 ==( void*) Cyc_Absyn_B2){ goto _LL179;} else{ goto
_LL180;}} else{ goto _LL180;} _LL180: if(( unsigned int) _temp174 > 4u?*(( int*)
_temp174) == Cyc_Absyn_IntType: 0){ _LL195: _temp194=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp174)->f1; goto _LL193; _LL193: _temp192=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp174)->f2; goto _LL181;} else{ goto _LL182;} _LL182: goto _LL183; _LL177: t=
_temp186 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp196=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp196[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp197; _temp197.tag= Cyc_Absyn_Const_e;
_temp197.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp198=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp198[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp199; _temp199.tag= Cyc_Absyn_Char_c;
_temp199.f1=( void*) _temp186; _temp199.f2=( unsigned char) _temp168; _temp199;});
_temp198;})); _temp197;}); _temp196;}))); goto _LL175; _LL179: t= _temp190 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp200=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp200[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp201; _temp201.tag= Cyc_Absyn_Const_e; _temp201.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp202=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp202[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp203; _temp203.tag= Cyc_Absyn_Short_c; _temp203.f1=( void*) _temp190;
_temp203.f2=( short) _temp168; _temp203;}); _temp202;})); _temp201;}); _temp200;})));
goto _LL175; _LL181: t= _temp194 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL175; _LL183: t= _temp170 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL175; _LL175:;} goto _LL139; _LL151: {
int len=( int) _get_arr_size( _temp172, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp208=({ struct Cyc_Absyn_Const_e_struct* _temp204=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp204[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp205; _temp205.tag= Cyc_Absyn_Const_e; _temp205.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp206=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp206[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp207; _temp207.tag= Cyc_Absyn_Int_c; _temp207.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp207.f2= len; _temp207;}); _temp206;})); _temp205;}); _temp204;}); goto
_LL209; _LL209: { struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*)
_temp208, loc); elen->topt=({ struct Cyc_Core_Opt* _temp210=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp210->v=( void*) Cyc_Absyn_uint_t;
_temp210;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp211=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp211[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp212; _temp212.tag= Cyc_Absyn_Upper_b;
_temp212.f1= elen; _temp212;}); _temp211;})); if( topt != 0){ void* _temp213=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_Exp*
_temp221; struct Cyc_Absyn_Tqual _temp223; void* _temp225; struct Cyc_Absyn_PtrInfo
_temp227; struct Cyc_Absyn_Conref* _temp229; struct Cyc_Absyn_Tqual _temp231;
struct Cyc_Absyn_Conref* _temp233; void* _temp235; void* _temp237; _LL215: if((
unsigned int) _temp213 > 4u?*(( int*) _temp213) == Cyc_Absyn_ArrayType: 0){
_LL226: _temp225=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp213)->f1;
goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_ArrayType_struct*) _temp213)->f2;
goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_ArrayType_struct*) _temp213)->f3;
goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp213 > 4u?*((
int*) _temp213) == Cyc_Absyn_PointerType: 0){ _LL228: _temp227=(( struct Cyc_Absyn_PointerType_struct*)
_temp213)->f1; _LL238: _temp237=( void*) _temp227.elt_typ; goto _LL236; _LL236:
_temp235=( void*) _temp227.rgn_typ; goto _LL234; _LL234: _temp233= _temp227.nullable;
goto _LL232; _LL232: _temp231= _temp227.tq; goto _LL230; _LL230: _temp229=
_temp227.bounds; goto _LL218;} else{ goto _LL219;} _LL219: goto _LL220; _LL216:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp239=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp239[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp240; _temp240.tag= Cyc_Absyn_ArrayType; _temp240.f1=(
void*) Cyc_Absyn_uchar_t; _temp240.f2= _temp223; _temp240.f3=( struct Cyc_Absyn_Exp*)
elen; _temp240;}); _temp239;}); _LL218: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp241=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp241->v=( void*) t; _temp241;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp242=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp242->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp242;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp243=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp243[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp244; _temp244.tag= Cyc_Absyn_Upper_b; _temp244.f1= elen; _temp244;});
_temp243;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp245=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp245->v=( void*) t; _temp245;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL214; _LL220:
goto _LL214; _LL214:;} return t;}} _LL153: { struct Cyc_List_List* _temp246= Cyc_Tcenv_lookup_type_vars(
te); goto _LL247; _LL247: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp248=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp248[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp249; _temp249.tag= Cyc_Absyn_PointerType;
_temp249.f1=({ struct Cyc_Absyn_PtrInfo _temp250; _temp250.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp252=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp252->v=( void*)
_temp246; _temp252;})); _temp250.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp251=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp251->v=( void*) _temp246;
_temp251;})); _temp250.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp250.tq= Cyc_Absyn_empty_tqual(); _temp250.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp250;}); _temp249;}); _temp248;}); goto _LL139;}
_LL139:;} return t;} static void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple1* q, void* b){ void* _temp253= b;
struct Cyc_Absyn_Vardecl* _temp267; struct Cyc_Absyn_Fndecl* _temp269; struct
Cyc_Absyn_Vardecl* _temp271; struct Cyc_Absyn_Vardecl* _temp273; struct Cyc_Absyn_Vardecl*
_temp275; _LL255: if( _temp253 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL256;}
else{ goto _LL257;} _LL257: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253)
== Cyc_Absyn_Global_b: 0){ _LL268: _temp267=(( struct Cyc_Absyn_Global_b_struct*)
_temp253)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Funname_b: 0){ _LL270: _temp269=((
struct Cyc_Absyn_Funname_b_struct*) _temp253)->f1; goto _LL260;} else{ goto
_LL261;} _LL261: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Pat_b:
0){ _LL272: _temp271=(( struct Cyc_Absyn_Pat_b_struct*) _temp253)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp253 > 1u?*(( int*)
_temp253) == Cyc_Absyn_Local_b: 0){ _LL274: _temp273=(( struct Cyc_Absyn_Local_b_struct*)
_temp253)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Param_b: 0){ _LL276: _temp275=((
struct Cyc_Absyn_Param_b_struct*) _temp253)->f1; goto _LL266;} else{ goto _LL254;}
_LL256: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp277= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_get_arr_size( _temp277, 1u), _temp277.curr);})); _LL258: q[
_check_known_subscript_notnull( 1u, 0)]=( _temp267->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp267->type; _LL260:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp269->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp269);
_LL262: _temp273= _temp271; goto _LL264; _LL264: _temp275= _temp273; goto _LL266;
_LL266:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp275->type; _LL254:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp278=( void*) fmt->r; void* _temp284; struct _tagged_arr
_temp286; _LL280: if(*(( int*) _temp278) == Cyc_Absyn_Const_e){ _LL285: _temp284=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp278)->f1; if(( unsigned int)
_temp284 > 1u?*(( int*) _temp284) == Cyc_Absyn_String_c: 0){ _LL287: _temp286=((
struct Cyc_Absyn_String_c_struct*) _temp284)->f1; goto _LL281;} else{ goto
_LL282;}} else{ goto _LL282;} _LL282: goto _LL283; _LL281: desc_types=
type_getter( te, _temp286, fmt->loc); goto _LL279; _LL283: Cyc_Tcutil_terr( fmt->loc,
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));
return; _LL279:;} for( 0; desc_types != 0? args != 0: 0;( desc_types=(( struct
Cyc_List_List*) _check_null( desc_types))->tl, args=(( struct Cyc_List_List*)
_check_null( args))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp288= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp289= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_get_arr_size( _temp288, 1u), _temp288.curr, _get_arr_size( _temp289, 1u),
_temp289.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments",
sizeof( unsigned char), 18u));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp290= p; _LL292: if( _temp290 ==( void*) Cyc_Absyn_Plus){ goto _LL293;}
else{ goto _LL294;} _LL294: if( _temp290 ==( void*) Cyc_Absyn_Minus){ goto
_LL295;} else{ goto _LL296;} _LL296: if( _temp290 ==( void*) Cyc_Absyn_Not){
goto _LL297;} else{ goto _LL298;} _LL298: if( _temp290 ==( void*) Cyc_Absyn_Bitnot){
goto _LL299;} else{ goto _LL300;} _LL300: if( _temp290 ==( void*) Cyc_Absyn_Size){
goto _LL301;} else{ goto _LL302;} _LL302: goto _LL303; _LL293: goto _LL295;
_LL295: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp304= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_get_arr_size( _temp304, 1u), _temp304.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL297: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp305= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_get_arr_size( _temp305, 1u), _temp305.curr);}));} return Cyc_Absyn_sint_t;
_LL299: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp306= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_get_arr_size( _temp306, 1u), _temp306.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL301:{ void* _temp307= t; struct Cyc_Absyn_Exp*
_temp315; struct Cyc_Absyn_Tqual _temp317; void* _temp319; struct Cyc_Absyn_PtrInfo
_temp321; struct Cyc_Absyn_Conref* _temp323; struct Cyc_Absyn_Tqual _temp325;
struct Cyc_Absyn_Conref* _temp327; void* _temp329; void* _temp331; _LL309: if((
unsigned int) _temp307 > 4u?*(( int*) _temp307) == Cyc_Absyn_ArrayType: 0){
_LL320: _temp319=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp307)->f1;
goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_ArrayType_struct*) _temp307)->f2;
goto _LL316; _LL316: _temp315=(( struct Cyc_Absyn_ArrayType_struct*) _temp307)->f3;
goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp307 > 4u?*((
int*) _temp307) == Cyc_Absyn_PointerType: 0){ _LL322: _temp321=(( struct Cyc_Absyn_PointerType_struct*)
_temp307)->f1; _LL332: _temp331=( void*) _temp321.elt_typ; goto _LL330; _LL330:
_temp329=( void*) _temp321.rgn_typ; goto _LL328; _LL328: _temp327= _temp321.nullable;
goto _LL326; _LL326: _temp325= _temp321.tq; goto _LL324; _LL324: _temp323=
_temp321.bounds; goto _LL312;} else{ goto _LL313;} _LL313: goto _LL314; _LL310:
goto _LL308; _LL312: goto _LL308; _LL314: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp333= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_get_arr_size( _temp333, 1u), _temp333.curr);})); goto _LL308; _LL308:;} return
Cyc_Absyn_uint_t; _LL303: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("Non-unary primop", sizeof( unsigned char), 17u)); _LL291:;} static
void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct Cyc_Absyn_Exp*)){ if( !
checker( e1)){ Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp334= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp334, 1u), _temp334.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp335=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp335->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp335;}));}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp336= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp336, 1u), _temp336.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp337=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp337->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp337;}));}{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp338= t1; struct Cyc_Absyn_PtrInfo _temp344; struct Cyc_Absyn_Conref*
_temp346; struct Cyc_Absyn_Tqual _temp348; struct Cyc_Absyn_Conref* _temp350;
void* _temp352; void* _temp354; _LL340: if(( unsigned int) _temp338 > 4u?*(( int*)
_temp338) == Cyc_Absyn_PointerType: 0){ _LL345: _temp344=(( struct Cyc_Absyn_PointerType_struct*)
_temp338)->f1; _LL355: _temp354=( void*) _temp344.elt_typ; goto _LL353; _LL353:
_temp352=( void*) _temp344.rgn_typ; goto _LL351; _LL351: _temp350= _temp344.nullable;
goto _LL349; _LL349: _temp348= _temp344.tq; goto _LL347; _LL347: _temp346=
_temp344.bounds; goto _LL341;} else{ goto _LL342;} _LL342: goto _LL343; _LL341:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp356= Cyc_Absynpp_typ2string( t2); xprintf("expecting int but found %.*s",
_get_arr_size( _temp356, 1u), _temp356.curr);}));} _temp346=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp346);{ void*
_temp357=( void*) _temp346->v; void* _temp365; void* _temp367; struct Cyc_Absyn_Exp*
_temp369; _LL359: if(( unsigned int) _temp357 > 1u?*(( int*) _temp357) == Cyc_Absyn_Eq_constr:
0){ _LL366: _temp365=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp357)->f1;
if( _temp365 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL360;} else{ goto _LL361;}}
else{ goto _LL361;} _LL361: if(( unsigned int) _temp357 > 1u?*(( int*) _temp357)
== Cyc_Absyn_Eq_constr: 0){ _LL368: _temp367=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp357)->f1; if(( unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Upper_b:
0){ _LL370: _temp369=(( struct Cyc_Absyn_Upper_b_struct*) _temp367)->f1; goto
_LL362;} else{ goto _LL363;}} else{ goto _LL363;} _LL363: goto _LL364; _LL360:
return t1; _LL362: { unsigned int _temp371= Cyc_Evexp_eval_const_uint_exp(
_temp369); goto _LL372; _LL372: _temp350=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp350);{ int is_nullable;{
void* _temp373=( void*) _temp350->v; int _temp379; _LL375: if(( unsigned int)
_temp373 > 1u?*(( int*) _temp373) == Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp373)->f1; goto _LL376;} else{
goto _LL377;} _LL377: goto _LL378; _LL376: if( _temp379){ struct Cyc_Absyn_PointerType_struct*
_temp384=({ struct Cyc_Absyn_PointerType_struct* _temp381=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp381[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp382; _temp382.tag= Cyc_Absyn_PointerType;
_temp382.f1=({ struct Cyc_Absyn_PtrInfo _temp383; _temp383.elt_typ=( void*)
_temp354; _temp383.rgn_typ=( void*) _temp352; _temp383.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp383.tq= _temp348; _temp383.bounds=
_temp346; _temp383;}); _temp382;}); _temp381;}); goto _LL385; _LL385: Cyc_Tcutil_warn(
e1->loc,( struct _tagged_arr)({ struct _tagged_arr _temp386= Cyc_Absynpp_typ2string(
t1); struct _tagged_arr _temp387= Cyc_Absynpp_typ2string(( void*) _temp384);
xprintf("coercing from %.*s to %.*s", _get_arr_size( _temp386, 1u), _temp386.curr,
_get_arr_size( _temp387, 1u), _temp387.curr);})); Cyc_Tcutil_unchecked_cast( te,
e1,( void*) _temp384);} goto _LL374; _LL378:( void*)( _temp350->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp388=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp389; _temp389.tag= Cyc_Absyn_Eq_constr; _temp389.f1=(
void*) 0; _temp389;}); _temp388;}))); goto _LL374; _LL374:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int _temp390= Cyc_Evexp_eval_const_uint_exp( e2); goto _LL391;
_LL391: if( _temp371 < _temp390){ _temp390= _temp371; Cyc_Tcutil_warn( e1->loc,(
struct _tagged_arr) xprintf("pointer arithmetic is out of bounds"));}{ struct
Cyc_Absyn_Exp* _temp392= Cyc_Absyn_uint_exp( _temp371 - _temp390, 0); goto
_LL393; _LL393: _temp392->topt=({ struct Cyc_Core_Opt* _temp394=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp394->v=( void*) Cyc_Absyn_uint_t;
_temp394;}); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp395=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp395[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp396; _temp396.tag= Cyc_Absyn_PointerType;
_temp396.f1=({ struct Cyc_Absyn_PtrInfo _temp397; _temp397.elt_typ=( void*)
_temp354; _temp397.rgn_typ=( void*) _temp352; _temp397.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp397.tq= _temp348; _temp397.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp398=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp398[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp399; _temp399.tag= Cyc_Absyn_Upper_b; _temp399.f1= _temp392; _temp399;});
_temp398;})); _temp397;}); _temp396;}); _temp395;});}} else{ Cyc_Tcutil_terr( e1->loc,
_tag_arr("cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.",
sizeof( unsigned char), 98u)); return t1;}}} _LL364:( void*)( _temp346->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp400=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp400[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp401; _temp401.tag= Cyc_Absyn_Eq_constr; _temp401.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp401;}); _temp400;}))); return t1;
_LL358:;} _LL343: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL339:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp402= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr _temp403=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_get_arr_size( _temp402, 1u), _temp402.curr, _get_arr_size( _temp403, 1u),
_temp403.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp404= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp405= Cyc_Absynpp_typ2string(
t2); xprintf("expecting either %.*s or int but found %.*s", _get_arr_size(
_temp404, 1u), _temp404.curr, _get_arr_size( _temp405, 1u), _temp405.curr);}));}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp406=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp406->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp406;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp408=({ struct _tuple0 _temp407;
_temp407.f1= Cyc_Tcutil_compress( t1); _temp407.f2= Cyc_Tcutil_compress( t2);
_temp407;}); void* _temp414; struct Cyc_Absyn_PtrInfo _temp416; struct Cyc_Absyn_Conref*
_temp418; struct Cyc_Absyn_Tqual _temp420; struct Cyc_Absyn_Conref* _temp422;
void* _temp424; void* _temp426; void* _temp428; struct Cyc_Absyn_PtrInfo
_temp430; struct Cyc_Absyn_Conref* _temp432; struct Cyc_Absyn_Tqual _temp434;
struct Cyc_Absyn_Conref* _temp436; void* _temp438; void* _temp440; _LL410:
_LL429: _temp428= _temp408.f1; if(( unsigned int) _temp428 > 4u?*(( int*)
_temp428) == Cyc_Absyn_PointerType: 0){ _LL431: _temp430=(( struct Cyc_Absyn_PointerType_struct*)
_temp428)->f1; _LL441: _temp440=( void*) _temp430.elt_typ; goto _LL439; _LL439:
_temp438=( void*) _temp430.rgn_typ; goto _LL437; _LL437: _temp436= _temp430.nullable;
goto _LL435; _LL435: _temp434= _temp430.tq; goto _LL433; _LL433: _temp432=
_temp430.bounds; goto _LL415;} else{ goto _LL412;} _LL415: _temp414= _temp408.f2;
if(( unsigned int) _temp414 > 4u?*(( int*) _temp414) == Cyc_Absyn_PointerType: 0){
_LL417: _temp416=(( struct Cyc_Absyn_PointerType_struct*) _temp414)->f1; _LL427:
_temp426=( void*) _temp416.elt_typ; goto _LL425; _LL425: _temp424=( void*)
_temp416.rgn_typ; goto _LL423; _LL423: _temp422= _temp416.nullable; goto _LL421;
_LL421: _temp420= _temp416.tq; goto _LL419; _LL419: _temp418= _temp416.bounds;
goto _LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL411: if( Cyc_Tcutil_unify(
_temp440, _temp426)){ return Cyc_Absyn_sint_t;} goto _LL409; _LL413: goto _LL409;
_LL409:;} Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp442= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp443= Cyc_Absynpp_typ2string(
t2); xprintf("comparison not allowed between %.*s and %.*s", _get_arr_size(
_temp442, 1u), _temp442.curr, _get_arr_size( _temp443, 1u), _temp443.curr);}));
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp444=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp444->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp444;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp445= p; _LL447: if( _temp445 ==( void*)
Cyc_Absyn_Plus){ goto _LL448;} else{ goto _LL449;} _LL449: if( _temp445 ==( void*)
Cyc_Absyn_Minus){ goto _LL450;} else{ goto _LL451;} _LL451: if( _temp445 ==(
void*) Cyc_Absyn_Times){ goto _LL452;} else{ goto _LL453;} _LL453: if( _temp445
==( void*) Cyc_Absyn_Div){ goto _LL454;} else{ goto _LL455;} _LL455: if(
_temp445 ==( void*) Cyc_Absyn_Mod){ goto _LL456;} else{ goto _LL457;} _LL457:
if( _temp445 ==( void*) Cyc_Absyn_Bitand){ goto _LL458;} else{ goto _LL459;}
_LL459: if( _temp445 ==( void*) Cyc_Absyn_Bitor){ goto _LL460;} else{ goto
_LL461;} _LL461: if( _temp445 ==( void*) Cyc_Absyn_Bitxor){ goto _LL462;} else{
goto _LL463;} _LL463: if( _temp445 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL464;}
else{ goto _LL465;} _LL465: if( _temp445 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL466;} else{ goto _LL467;} _LL467: if( _temp445 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL468;} else{ goto _LL469;} _LL469: if( _temp445 ==( void*) Cyc_Absyn_Eq){
goto _LL470;} else{ goto _LL471;} _LL471: if( _temp445 ==( void*) Cyc_Absyn_Neq){
goto _LL472;} else{ goto _LL473;} _LL473: if( _temp445 ==( void*) Cyc_Absyn_Gt){
goto _LL474;} else{ goto _LL475;} _LL475: if( _temp445 ==( void*) Cyc_Absyn_Lt){
goto _LL476;} else{ goto _LL477;} _LL477: if( _temp445 ==( void*) Cyc_Absyn_Gte){
goto _LL478;} else{ goto _LL479;} _LL479: if( _temp445 ==( void*) Cyc_Absyn_Lte){
goto _LL480;} else{ goto _LL481;} _LL481: goto _LL482; _LL448: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL450: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL452: goto _LL454;
_LL454: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL456: goto _LL458; _LL458: goto _LL460; _LL460: goto _LL462; _LL462: goto
_LL464; _LL464: goto _LL466; _LL466: goto _LL468; _LL468: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL470: goto _LL472; _LL472: goto _LL474;
_LL474: goto _LL476; _LL476: goto _LL478; _LL478: goto _LL480; _LL480: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL482:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp483=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp483[ 0]=({ struct Cyc_Core_Failure_struct
_temp484; _temp484.tag= Cyc_Core_Failure; _temp484.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp484;}); _temp483;})); _LL446:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp485= p; _LL487: if( _temp485
==( void*) Cyc_Absyn_Printf){ goto _LL488;} else{ goto _LL489;} _LL489: if(
_temp485 ==( void*) Cyc_Absyn_Fprintf){ goto _LL490;} else{ goto _LL491;} _LL491:
if( _temp485 ==( void*) Cyc_Absyn_Xprintf){ goto _LL492;} else{ goto _LL493;}
_LL493: if( _temp485 ==( void*) Cyc_Absyn_Scanf){ goto _LL494;} else{ goto
_LL495;} _LL495: if( _temp485 ==( void*) Cyc_Absyn_Fscanf){ goto _LL496;} else{
goto _LL497;} _LL497: if( _temp485 ==( void*) Cyc_Absyn_Sscanf){ goto _LL498;}
else{ goto _LL499;} _LL499: goto _LL500; _LL488: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("missing format string in printf", sizeof( unsigned char), 32u));}
Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd,(( struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL486; _LL490: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fprintf",
sizeof( unsigned char), 29u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp501=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp501, 1u),
_temp501.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL486;
_LL492: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in xprintf",
sizeof( unsigned char), 33u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt*
_temp502=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp502->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp502;}))); goto _LL486;
_LL494: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in scanf",
sizeof( unsigned char), 31u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL486; _LL496: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fscanf",
sizeof( unsigned char), 28u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp503=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp503, 1u),
_temp503.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL486; _LL498: Cyc_Tcutil_terr(
loc, _tag_arr("sscanf is not supported", sizeof( unsigned char), 24u)); t= Cyc_Absyn_sint_t;
goto _LL486; _LL500: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ case 0: _LL504: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL505: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL506: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL507: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL486; _LL486:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp509=( void*) e->r;
void* _temp533; struct Cyc_Absyn_Vardecl* _temp535; struct _tuple1* _temp537;
void* _temp539; struct Cyc_Absyn_Vardecl* _temp541; struct _tuple1* _temp543;
void* _temp545; struct Cyc_Absyn_Vardecl* _temp547; struct _tuple1* _temp549;
void* _temp551; struct Cyc_Absyn_Vardecl* _temp553; struct _tuple1* _temp555;
struct Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp* _temp559; struct
_tagged_arr* _temp561; struct Cyc_Absyn_Exp* _temp563; struct _tagged_arr*
_temp565; struct Cyc_Absyn_Exp* _temp567; struct Cyc_Absyn_Exp* _temp569; struct
Cyc_Absyn_Exp* _temp571; struct Cyc_List_List* _temp573; struct Cyc_Absyn_Exp*
_temp575; _LL511: if(*(( int*) _temp509) == Cyc_Absyn_Var_e){ _LL538: _temp537=((
struct Cyc_Absyn_Var_e_struct*) _temp509)->f1; goto _LL534; _LL534: _temp533=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f2; if(( unsigned int)
_temp533 > 1u?*(( int*) _temp533) == Cyc_Absyn_Param_b: 0){ _LL536: _temp535=((
struct Cyc_Absyn_Param_b_struct*) _temp533)->f1; goto _LL512;} else{ goto _LL513;}}
else{ goto _LL513;} _LL513: if(*(( int*) _temp509) == Cyc_Absyn_Var_e){ _LL544:
_temp543=(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f1; goto _LL540; _LL540:
_temp539=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f2; if((
unsigned int) _temp539 > 1u?*(( int*) _temp539) == Cyc_Absyn_Local_b: 0){ _LL542:
_temp541=(( struct Cyc_Absyn_Local_b_struct*) _temp539)->f1; goto _LL514;} else{
goto _LL515;}} else{ goto _LL515;} _LL515: if(*(( int*) _temp509) == Cyc_Absyn_Var_e){
_LL550: _temp549=(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f1; goto _LL546;
_LL546: _temp545=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f2; if((
unsigned int) _temp545 > 1u?*(( int*) _temp545) == Cyc_Absyn_Pat_b: 0){ _LL548:
_temp547=(( struct Cyc_Absyn_Pat_b_struct*) _temp545)->f1; goto _LL516;} else{
goto _LL517;}} else{ goto _LL517;} _LL517: if(*(( int*) _temp509) == Cyc_Absyn_Var_e){
_LL556: _temp555=(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f1; goto _LL552;
_LL552: _temp551=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp509)->f2; if((
unsigned int) _temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_Global_b: 0){
_LL554: _temp553=(( struct Cyc_Absyn_Global_b_struct*) _temp551)->f1; goto
_LL518;} else{ goto _LL519;}} else{ goto _LL519;} _LL519: if(*(( int*) _temp509)
== Cyc_Absyn_Subscript_e){ _LL560: _temp559=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp509)->f1; goto _LL558; _LL558: _temp557=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp509)->f2; goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp509)
== Cyc_Absyn_StructMember_e){ _LL564: _temp563=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp509)->f1; goto _LL562; _LL562: _temp561=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp509)->f2; goto _LL522;} else{ goto _LL523;} _LL523: if(*(( int*) _temp509)
== Cyc_Absyn_StructArrow_e){ _LL568: _temp567=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp509)->f1; goto _LL566; _LL566: _temp565=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp509)->f2; goto _LL524;} else{ goto _LL525;} _LL525: if(*(( int*) _temp509)
== Cyc_Absyn_Deref_e){ _LL570: _temp569=(( struct Cyc_Absyn_Deref_e_struct*)
_temp509)->f1; goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp509)
== Cyc_Absyn_NoInstantiate_e){ _LL572: _temp571=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp509)->f1; goto _LL528;} else{ goto _LL529;} _LL529: if(*(( int*) _temp509)
== Cyc_Absyn_Instantiate_e){ _LL576: _temp575=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp509)->f1; goto _LL574; _LL574: _temp573=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp509)->f2; goto _LL530;} else{ goto _LL531;} _LL531: goto _LL532; _LL512:
_temp541= _temp535; goto _LL514; _LL514: _temp547= _temp541; goto _LL516; _LL516:
_temp553= _temp547; goto _LL518; _LL518: if( !( _temp553->tq).q_const){ return;}
goto _LL510; _LL520:{ void* _temp577= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp559->topt))->v); struct Cyc_Absyn_PtrInfo _temp587; struct Cyc_Absyn_Conref*
_temp589; struct Cyc_Absyn_Tqual _temp591; struct Cyc_Absyn_Conref* _temp593;
void* _temp595; void* _temp597; struct Cyc_Absyn_Exp* _temp599; struct Cyc_Absyn_Tqual
_temp601; void* _temp603; struct Cyc_List_List* _temp605; _LL579: if((
unsigned int) _temp577 > 4u?*(( int*) _temp577) == Cyc_Absyn_PointerType: 0){
_LL588: _temp587=(( struct Cyc_Absyn_PointerType_struct*) _temp577)->f1; _LL598:
_temp597=( void*) _temp587.elt_typ; goto _LL596; _LL596: _temp595=( void*)
_temp587.rgn_typ; goto _LL594; _LL594: _temp593= _temp587.nullable; goto _LL592;
_LL592: _temp591= _temp587.tq; goto _LL590; _LL590: _temp589= _temp587.bounds;
goto _LL580;} else{ goto _LL581;} _LL581: if(( unsigned int) _temp577 > 4u?*((
int*) _temp577) == Cyc_Absyn_ArrayType: 0){ _LL604: _temp603=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp577)->f1; goto _LL602; _LL602: _temp601=((
struct Cyc_Absyn_ArrayType_struct*) _temp577)->f2; goto _LL600; _LL600: _temp599=((
struct Cyc_Absyn_ArrayType_struct*) _temp577)->f3; goto _LL582;} else{ goto
_LL583;} _LL583: if(( unsigned int) _temp577 > 4u?*(( int*) _temp577) == Cyc_Absyn_TupleType:
0){ _LL606: _temp605=(( struct Cyc_Absyn_TupleType_struct*) _temp577)->f1; goto
_LL584;} else{ goto _LL585;} _LL585: goto _LL586; _LL580: _temp601= _temp591;
goto _LL582; _LL582: if( ! _temp601.q_const){ return;} goto _LL578; _LL584: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp557);{ struct _handler_cons
_temp607; _push_handler(& _temp607);{ int _temp609= 0; if( setjmp( _temp607.handler)){
_temp609= 1;} if( ! _temp609){{ struct _tuple8 _temp612; void* _temp613; struct
Cyc_Absyn_Tqual _temp615; struct _tuple8* _temp610=(( struct _tuple8*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp605,( int) i); _temp612=* _temp610;
_LL616: _temp615= _temp612.f1; goto _LL614; _LL614: _temp613= _temp612.f2; goto
_LL611; _LL611: if( ! _temp615.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp608=( void*) _exn_thrown; void* _temp618=
_temp608; _LL620: if( _temp618 == Cyc_List_Nth){ goto _LL621;} else{ goto _LL622;}
_LL622: goto _LL623; _LL621: return; _LL623:( void) _throw( _temp618); _LL619:;}}}
goto _LL578;} _LL586: goto _LL578; _LL578:;} goto _LL510; _LL522:{ void*
_temp624= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp563->topt))->v); struct Cyc_Absyn_Structdecl** _temp636; struct Cyc_List_List*
_temp638; struct _tuple1* _temp640; struct Cyc_List_List* _temp642; struct Cyc_List_List*
_temp644; struct Cyc_Absyn_Uniondecl** _temp646; struct Cyc_List_List* _temp648;
struct _tuple1* _temp650; _LL626: if(( unsigned int) _temp624 > 4u?*(( int*)
_temp624) == Cyc_Absyn_StructType: 0){ _LL641: _temp640=(( struct Cyc_Absyn_StructType_struct*)
_temp624)->f1; goto _LL639; _LL639: _temp638=(( struct Cyc_Absyn_StructType_struct*)
_temp624)->f2; goto _LL637; _LL637: _temp636=(( struct Cyc_Absyn_StructType_struct*)
_temp624)->f3; goto _LL627;} else{ goto _LL628;} _LL628: if(( unsigned int)
_temp624 > 4u?*(( int*) _temp624) == Cyc_Absyn_AnonUnionType: 0){ _LL643:
_temp642=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp624)->f1; goto _LL629;}
else{ goto _LL630;} _LL630: if(( unsigned int) _temp624 > 4u?*(( int*) _temp624)
== Cyc_Absyn_AnonStructType: 0){ _LL645: _temp644=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp624)->f1; goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int)
_temp624 > 4u?*(( int*) _temp624) == Cyc_Absyn_UnionType: 0){ _LL651: _temp650=((
struct Cyc_Absyn_UnionType_struct*) _temp624)->f1; goto _LL649; _LL649: _temp648=((
struct Cyc_Absyn_UnionType_struct*) _temp624)->f2; goto _LL647; _LL647: _temp646=((
struct Cyc_Absyn_UnionType_struct*) _temp624)->f3; goto _LL633;} else{ goto
_LL634;} _LL634: goto _LL635; _LL627: { struct Cyc_Absyn_Structfield* sf=
_temp636 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp636)), _temp561); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL625;} _LL629: _temp644= _temp642; goto _LL631; _LL631: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp644, _temp561); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL625;} _LL633: { struct Cyc_Absyn_Structfield*
sf= _temp646 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp646)), _temp561); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL625;} _LL635: goto _LL625; _LL625:;} goto _LL510; _LL524:{ void*
_temp652= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp567->topt))->v); struct Cyc_Absyn_PtrInfo _temp658; struct Cyc_Absyn_Conref*
_temp660; struct Cyc_Absyn_Tqual _temp662; struct Cyc_Absyn_Conref* _temp664;
void* _temp666; void* _temp668; _LL654: if(( unsigned int) _temp652 > 4u?*(( int*)
_temp652) == Cyc_Absyn_PointerType: 0){ _LL659: _temp658=(( struct Cyc_Absyn_PointerType_struct*)
_temp652)->f1; _LL669: _temp668=( void*) _temp658.elt_typ; goto _LL667; _LL667:
_temp666=( void*) _temp658.rgn_typ; goto _LL665; _LL665: _temp664= _temp658.nullable;
goto _LL663; _LL663: _temp662= _temp658.tq; goto _LL661; _LL661: _temp660=
_temp658.bounds; goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL655:{
void* _temp670= Cyc_Tcutil_compress( _temp668); struct Cyc_Absyn_Structdecl**
_temp682; struct Cyc_List_List* _temp684; struct _tuple1* _temp686; struct Cyc_Absyn_Uniondecl**
_temp688; struct Cyc_List_List* _temp690; struct _tuple1* _temp692; struct Cyc_List_List*
_temp694; struct Cyc_List_List* _temp696; _LL672: if(( unsigned int) _temp670 >
4u?*(( int*) _temp670) == Cyc_Absyn_StructType: 0){ _LL687: _temp686=(( struct
Cyc_Absyn_StructType_struct*) _temp670)->f1; goto _LL685; _LL685: _temp684=((
struct Cyc_Absyn_StructType_struct*) _temp670)->f2; goto _LL683; _LL683:
_temp682=(( struct Cyc_Absyn_StructType_struct*) _temp670)->f3; goto _LL673;}
else{ goto _LL674;} _LL674: if(( unsigned int) _temp670 > 4u?*(( int*) _temp670)
== Cyc_Absyn_UnionType: 0){ _LL693: _temp692=(( struct Cyc_Absyn_UnionType_struct*)
_temp670)->f1; goto _LL691; _LL691: _temp690=(( struct Cyc_Absyn_UnionType_struct*)
_temp670)->f2; goto _LL689; _LL689: _temp688=(( struct Cyc_Absyn_UnionType_struct*)
_temp670)->f3; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp670 > 4u?*(( int*) _temp670) == Cyc_Absyn_AnonStructType: 0){ _LL695:
_temp694=(( struct Cyc_Absyn_AnonStructType_struct*) _temp670)->f1; goto _LL677;}
else{ goto _LL678;} _LL678: if(( unsigned int) _temp670 > 4u?*(( int*) _temp670)
== Cyc_Absyn_AnonUnionType: 0){ _LL697: _temp696=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp670)->f1; goto _LL679;} else{ goto _LL680;} _LL680: goto _LL681; _LL673: {
struct Cyc_Absyn_Structfield* sf= _temp682 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp682)), _temp565); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL671;} _LL675: { struct Cyc_Absyn_Structfield*
sf= _temp688 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp688)), _temp565); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL671;} _LL677: _temp696= _temp694; goto _LL679; _LL679: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp696, _temp565); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL671;} _LL681: goto _LL671;
_LL671:;} goto _LL653; _LL657: goto _LL653; _LL653:;} goto _LL510; _LL526:{ void*
_temp698= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp569->topt))->v); struct Cyc_Absyn_PtrInfo _temp706; struct Cyc_Absyn_Conref*
_temp708; struct Cyc_Absyn_Tqual _temp710; struct Cyc_Absyn_Conref* _temp712;
void* _temp714; void* _temp716; struct Cyc_Absyn_Exp* _temp718; struct Cyc_Absyn_Tqual
_temp720; void* _temp722; _LL700: if(( unsigned int) _temp698 > 4u?*(( int*)
_temp698) == Cyc_Absyn_PointerType: 0){ _LL707: _temp706=(( struct Cyc_Absyn_PointerType_struct*)
_temp698)->f1; _LL717: _temp716=( void*) _temp706.elt_typ; goto _LL715; _LL715:
_temp714=( void*) _temp706.rgn_typ; goto _LL713; _LL713: _temp712= _temp706.nullable;
goto _LL711; _LL711: _temp710= _temp706.tq; goto _LL709; _LL709: _temp708=
_temp706.bounds; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp698 > 4u?*(( int*) _temp698) == Cyc_Absyn_ArrayType: 0){ _LL723: _temp722=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp698)->f1; goto _LL721; _LL721:
_temp720=(( struct Cyc_Absyn_ArrayType_struct*) _temp698)->f2; goto _LL719;
_LL719: _temp718=(( struct Cyc_Absyn_ArrayType_struct*) _temp698)->f3; goto
_LL703;} else{ goto _LL704;} _LL704: goto _LL705; _LL701: _temp720= _temp710;
goto _LL703; _LL703: if( ! _temp720.q_const){ return;} goto _LL699; _LL705: goto
_LL699; _LL699:;} goto _LL510; _LL528: _temp575= _temp571; goto _LL530; _LL530:
Cyc_Tcexp_check_writable( te, _temp575); return; _LL532: goto _LL510; _LL510:;}
Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp724= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_get_arr_size( _temp724, 1u), _temp724.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp733=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp733->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp733;})); struct Cyc_List_List* _temp726=({
struct Cyc_List_List* _temp725=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp725->hd=( void*) e3; _temp725->tl= 0; _temp725;});
goto _LL727; _LL727: { struct Cyc_List_List* _temp729=({ struct Cyc_List_List*
_temp728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp728->hd=( void*) e2; _temp728->tl= _temp726; _temp728;}); goto _LL730;
_LL730: if( ! Cyc_Tcutil_coerce_list( te, t, _temp729)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp731= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); struct _tagged_arr _temp732=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e3->topt))->v);
xprintf("conditional clause types do not match: %.*s != %.*s", _get_arr_size(
_temp731, 1u), _temp731.curr, _get_arr_size( _temp732, 1u), _temp732.curr);}));}
return t;}}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1);
Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp734= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp740; struct Cyc_Absyn_Tqual _temp742; void* _temp744; _LL736:
if(( unsigned int) _temp734 > 4u?*(( int*) _temp734) == Cyc_Absyn_ArrayType: 0){
_LL745: _temp744=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp734)->f1;
goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_ArrayType_struct*) _temp734)->f2;
goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_ArrayType_struct*) _temp734)->f3;
goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL737: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL735; _LL739: goto _LL735; _LL735:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp746= Cyc_Absynpp_typ2string( t1);
struct _tagged_arr _temp747= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_get_arr_size( _temp746, 1u), _temp746.curr, _get_arr_size( _temp747, 1u),
_temp747.curr);}));}} else{ void* _temp748=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; goto _LL749; _LL749: { void* _temp750= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp748, e1, e2); goto _LL751; _LL751: if( !( Cyc_Tcutil_unify(
_temp750, t1)? 1: Cyc_Tcutil_coerceable( _temp750))){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("Cannot use this operator in an assignment", sizeof(
unsigned char), 42u));} return _temp750;}} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; for( fields= fs; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp754;
struct Cyc_Position_Segment* _temp756; struct Cyc_List_List* _temp758; struct
Cyc_List_List* _temp760; struct _tuple1* _temp762; struct Cyc_Absyn_Tunionfield
_temp752=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL763: _temp762= _temp752.name; goto _LL761; _LL761: _temp760=
_temp752.tvs; goto _LL759; _LL759: _temp758= _temp752.typs; goto _LL757; _LL757:
_temp756= _temp752.loc; goto _LL755; _LL755: _temp754=( void*) _temp752.sc; goto
_LL753; _LL753: if(( _temp758 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp758))->tl != 0)? 1: _temp760 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp758))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp766; struct Cyc_Position_Segment* _temp768; struct Cyc_List_List*
_temp770; struct Cyc_List_List* _temp772; struct _tuple1* _temp774; struct Cyc_Absyn_Tunionfield
_temp764=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL775: _temp774= _temp764.name; goto _LL773; _LL773: _temp772=
_temp764.tvs; goto _LL771; _LL771: _temp770= _temp764.typs; goto _LL769; _LL769:
_temp768= _temp764.loc; goto _LL767; _LL767: _temp766=( void*) _temp764.sc; goto
_LL765; _LL765: if(( _temp770 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp770))->tl != 0)? 1: _temp772 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp770))->hd)).f2);
if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp776= Cyc_Absynpp_typ2string( tu);
struct _tagged_arr _temp777= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_get_arr_size( _temp776, 1u), _temp776.curr, _get_arr_size( _temp777, 1u),
_temp777.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){
te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp778= t;
struct Cyc_Absyn_PtrInfo _temp784; struct Cyc_Absyn_Conref* _temp786; struct Cyc_Absyn_Tqual
_temp788; struct Cyc_Absyn_Conref* _temp790; void* _temp792; void* _temp794;
_LL780: if(( unsigned int) _temp778 > 4u?*(( int*) _temp778) == Cyc_Absyn_PointerType:
0){ _LL785: _temp784=(( struct Cyc_Absyn_PointerType_struct*) _temp778)->f1;
_LL795: _temp794=( void*) _temp784.elt_typ; goto _LL793; _LL793: _temp792=( void*)
_temp784.rgn_typ; goto _LL791; _LL791: _temp790= _temp784.nullable; goto _LL789;
_LL789: _temp788= _temp784.tq; goto _LL787; _LL787: _temp786= _temp784.bounds;
goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp792); Cyc_Tcutil_check_nonzero_bound( loc, _temp786);{ void*
_temp796= Cyc_Tcutil_compress( _temp794); struct Cyc_Absyn_FnInfo _temp802;
struct Cyc_List_List* _temp804; struct Cyc_List_List* _temp806; struct Cyc_Absyn_VarargInfo*
_temp808; int _temp810; struct Cyc_List_List* _temp812; void* _temp814; struct
Cyc_Core_Opt* _temp816; struct Cyc_List_List* _temp818; _LL798: if((
unsigned int) _temp796 > 4u?*(( int*) _temp796) == Cyc_Absyn_FnType: 0){ _LL803:
_temp802=(( struct Cyc_Absyn_FnType_struct*) _temp796)->f1; _LL819: _temp818=
_temp802.tvars; goto _LL817; _LL817: _temp816= _temp802.effect; goto _LL815;
_LL815: _temp814=( void*) _temp802.ret_typ; goto _LL813; _LL813: _temp812=
_temp802.args; goto _LL811; _LL811: _temp810= _temp802.c_varargs; goto _LL809;
_LL809: _temp808= _temp802.cyc_varargs; goto _LL807; _LL807: _temp806= _temp802.rgn_po;
goto _LL805; _LL805: _temp804= _temp802.attributes; goto _LL799;} else{ goto
_LL800;} _LL800: goto _LL801; _LL799: if( topt != 0){ Cyc_Tcutil_unify( _temp814,*((
void**) _check_null( topt)));} while( es != 0? _temp812 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp812))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp820= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr
_temp821= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_get_arr_size( _temp820, 1u), _temp820.curr, _get_arr_size( _temp821, 1u),
_temp821.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp812=(( struct Cyc_List_List*) _check_null( _temp812))->tl;} if( _temp812 !=
0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function", sizeof(
unsigned char), 31u));} else{ if( es != 0){ if( _temp810){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}} else{ if(
_temp808 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function",
sizeof( unsigned char), 32u));} else{ int _temp824; void* _temp826; void*
_temp828; struct Cyc_Absyn_Tqual _temp830; struct Cyc_Core_Opt* _temp832; struct
Cyc_Absyn_VarargInfo _temp822=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp808)); _LL833: _temp832= _temp822.name; goto _LL831; _LL831: _temp830=
_temp822.tq; goto _LL829; _LL829: _temp828=( void*) _temp822.type; goto _LL827;
_LL827: _temp826=( void*) _temp822.rgn; goto _LL825; _LL825: _temp824= _temp822.inject;
goto _LL823; _LL823: { struct Cyc_Absyn_VarargCallInfo* _temp835=({ struct Cyc_Absyn_VarargCallInfo*
_temp834=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp834->num_varargs= 0; _temp834->injectors= 0; _temp834->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp808); _temp834;}); goto _LL836; _LL836:* vararg_call_info=
_temp835; if( ! Cyc_Tcutil_unify( Cyc_Tcenv_curr_rgn( te), _temp826)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp837= Cyc_Absynpp_typ2string(
_temp826); struct _tagged_arr _temp838= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn(
te)); xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_get_arr_size( _temp837, 1u), _temp837.curr, _get_arr_size( _temp838, 1u),
_temp838.curr);}));} if( ! _temp824){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp835))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp828,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp828)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp839= Cyc_Absynpp_typ2string(
_temp828); struct _tagged_arr _temp840= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_get_arr_size( _temp839, 1u), _temp839.curr, _get_arr_size( _temp840, 1u),
_temp840.curr);}));}}} else{ void* _temp841= Cyc_Tcutil_compress( _temp828);
struct Cyc_Absyn_TunionInfo _temp847; void* _temp849; struct Cyc_List_List*
_temp851; void* _temp853; struct Cyc_Absyn_Tuniondecl* _temp855; _LL843: if((
unsigned int) _temp841 > 4u?*(( int*) _temp841) == Cyc_Absyn_TunionType: 0){
_LL848: _temp847=(( struct Cyc_Absyn_TunionType_struct*) _temp841)->f1; _LL854:
_temp853=( void*) _temp847.tunion_info; if(*(( int*) _temp853) == Cyc_Absyn_KnownTunion){
_LL856: _temp855=(( struct Cyc_Absyn_KnownTunion_struct*) _temp853)->f1; goto
_LL852;} else{ goto _LL845;} _LL852: _temp851= _temp847.targs; goto _LL850;
_LL850: _temp849=( void*) _temp847.rgn; goto _LL844;} else{ goto _LL845;} _LL845:
goto _LL846; _LL844: { struct Cyc_Absyn_Tuniondecl* _temp857=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp855->name); goto _LL858; _LL858: if( ! Cyc_Tcutil_unify( _temp849,
_temp826)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("bad region for injected varargs"));}{
struct Cyc_List_List* fields= 0; if( _temp857->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp859= Cyc_Absynpp_typ2string(
_temp828); xprintf("can't inject into %.*s", _get_arr_size( _temp859, 1u),
_temp859.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp857->fields))->v;}{ struct _RegionHandle _temp860= _new_region();
struct _RegionHandle* rgn=& _temp860; _push_region( rgn);{ struct Cyc_List_List*
_temp861=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp857->tvs, _temp851); goto _LL862; _LL862: for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp835))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp863= Cyc_Tcexp_tcInjection( te, e1, _temp828,
rgn, _temp861, fields); goto _LL864; _LL864: if( _temp863 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp835))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp835))->injectors,({ struct Cyc_List_List* _temp865=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp865->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp863)); _temp865->tl= 0;
_temp865;}));}}}}}; _pop_region( rgn);} goto _LL842;}} _LL846: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL842; _LL842:;}}}}}} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp816))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp806); return _temp814; _LL801: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL797:;}
_LL783: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL779:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp866= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_get_arr_size( _temp866, 1u), _temp866.curr);}));} return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp867->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp867;}));}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp868= t1; struct Cyc_Absyn_PtrInfo
_temp874; struct Cyc_Absyn_Conref* _temp876; struct Cyc_Absyn_Tqual _temp878;
struct Cyc_Absyn_Conref* _temp880; void* _temp882; void* _temp884; _LL870: if((
unsigned int) _temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_PointerType: 0){
_LL875: _temp874=(( struct Cyc_Absyn_PointerType_struct*) _temp868)->f1; _LL885:
_temp884=( void*) _temp874.elt_typ; goto _LL883; _LL883: _temp882=( void*)
_temp874.rgn_typ; goto _LL881; _LL881: _temp880= _temp874.nullable; goto _LL879;
_LL879: _temp878= _temp874.tq; goto _LL877; _LL877: _temp876= _temp874.bounds;
goto _LL871;} else{ goto _LL872;} _LL872: goto _LL873; _LL871:{ void* _temp886=
Cyc_Tcutil_compress( _temp884); struct Cyc_Absyn_FnInfo _temp892; struct Cyc_List_List*
_temp894; struct Cyc_List_List* _temp896; struct Cyc_Absyn_VarargInfo* _temp898;
int _temp900; struct Cyc_List_List* _temp902; void* _temp904; struct Cyc_Core_Opt*
_temp906; struct Cyc_List_List* _temp908; _LL888: if(( unsigned int) _temp886 >
4u?*(( int*) _temp886) == Cyc_Absyn_FnType: 0){ _LL893: _temp892=(( struct Cyc_Absyn_FnType_struct*)
_temp886)->f1; _LL909: _temp908= _temp892.tvars; goto _LL907; _LL907: _temp906=
_temp892.effect; goto _LL905; _LL905: _temp904=( void*) _temp892.ret_typ; goto
_LL903; _LL903: _temp902= _temp892.args; goto _LL901; _LL901: _temp900= _temp892.c_varargs;
goto _LL899; _LL899: _temp898= _temp892.cyc_varargs; goto _LL897; _LL897:
_temp896= _temp892.rgn_po; goto _LL895; _LL895: _temp894= _temp892.attributes;
goto _LL889;} else{ goto _LL890;} _LL890: goto _LL891; _LL889: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp908)){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u));}
for( 0; ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, _temp908=((
struct Cyc_List_List*) _check_null( _temp908))->tl)){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp908))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp910=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp910->hd=( void*)({
struct _tuple5* _temp911=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp911->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp908))->hd; _temp911->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp911;}); _temp910->tl= instantiation; _temp910;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp915=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp915[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp916; _temp916.tag= Cyc_Absyn_FnType; _temp916.f1=({ struct Cyc_Absyn_FnInfo
_temp917; _temp917.tvars= 0; _temp917.effect= _temp906; _temp917.ret_typ=( void*)
_temp904; _temp917.args= _temp902; _temp917.c_varargs= _temp900; _temp917.cyc_varargs=
_temp898; _temp917.rgn_po= _temp896; _temp917.attributes= _temp894; _temp917;});
_temp916;}); _temp915;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp912=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp912[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp913; _temp913.tag= Cyc_Absyn_PointerType;
_temp913.f1=({ struct Cyc_Absyn_PtrInfo _temp914; _temp914.elt_typ=( void*)
new_fn_typ; _temp914.rgn_typ=( void*) _temp882; _temp914.nullable= _temp880;
_temp914.tq= _temp878; _temp914.bounds= _temp876; _temp914;}); _temp913;});
_temp912;}); return new_typ;}} _LL891: goto _LL887; _LL887:;} goto _LL869;
_LL873: goto _LL869; _LL869:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp918= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_get_arr_size( _temp918, 1u), _temp918.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp919= Cyc_Absynpp_typ2string(
t2); struct _tagged_arr _temp920= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_get_arr_size( _temp919, 1u), _temp919.curr, _get_arr_size( _temp920, 1u),
_temp920.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp921= 0; goto _LL922; _LL922: { struct Cyc_Absyn_Tqual _temp923= Cyc_Absyn_empty_tqual();
goto _LL924; _LL924: if( topt != 0){ void* _temp925= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp931; struct Cyc_Absyn_Conref*
_temp933; struct Cyc_Absyn_Tqual _temp935; struct Cyc_Absyn_Conref* _temp937;
void* _temp939; void* _temp941; _LL927: if(( unsigned int) _temp925 > 4u?*(( int*)
_temp925) == Cyc_Absyn_PointerType: 0){ _LL932: _temp931=(( struct Cyc_Absyn_PointerType_struct*)
_temp925)->f1; _LL942: _temp941=( void*) _temp931.elt_typ; goto _LL940; _LL940:
_temp939=( void*) _temp931.rgn_typ; goto _LL938; _LL938: _temp937= _temp931.nullable;
goto _LL936; _LL936: _temp935= _temp931.tq; goto _LL934; _LL934: _temp933=
_temp931.bounds; goto _LL928;} else{ goto _LL929;} _LL929: goto _LL930; _LL928:
_temp921=({ void** _temp943=( void**) GC_malloc( sizeof( void*)); _temp943[ 0]=
_temp941; _temp943;}); _temp923= _temp935; goto _LL926; _LL930: goto _LL926;
_LL926:;} Cyc_Tcexp_tcExpNoInst( te, _temp921, e);{ void* _temp944=( void*) e->r;
struct Cyc_Absyn_Structdecl* _temp952; struct Cyc_List_List* _temp954; struct
Cyc_Core_Opt* _temp956; struct _tuple1* _temp958; struct Cyc_List_List* _temp960;
_LL946: if(*(( int*) _temp944) == Cyc_Absyn_Struct_e){ _LL959: _temp958=((
struct Cyc_Absyn_Struct_e_struct*) _temp944)->f1; goto _LL957; _LL957: _temp956=((
struct Cyc_Absyn_Struct_e_struct*) _temp944)->f2; goto _LL955; _LL955: _temp954=((
struct Cyc_Absyn_Struct_e_struct*) _temp944)->f3; goto _LL953; _LL953: _temp952=((
struct Cyc_Absyn_Struct_e_struct*) _temp944)->f4; goto _LL947;} else{ goto
_LL948;} _LL948: if(*(( int*) _temp944) == Cyc_Absyn_Tuple_e){ _LL961: _temp960=((
struct Cyc_Absyn_Tuple_e_struct*) _temp944)->f1; goto _LL949;} else{ goto _LL950;}
_LL950: goto _LL951; _LL947: goto _LL949; _LL949: Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u));{ void*(* _temp962)( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL963; _LL963: if( topt != 0){ void*
_temp964= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp970; struct Cyc_Absyn_Conref* _temp972; struct Cyc_Absyn_Tqual _temp974;
struct Cyc_Absyn_Conref* _temp976; void* _temp978; void* _temp980; _LL966: if((
unsigned int) _temp964 > 4u?*(( int*) _temp964) == Cyc_Absyn_PointerType: 0){
_LL971: _temp970=(( struct Cyc_Absyn_PointerType_struct*) _temp964)->f1; _LL981:
_temp980=( void*) _temp970.elt_typ; goto _LL979; _LL979: _temp978=( void*)
_temp970.rgn_typ; goto _LL977; _LL977: _temp976= _temp970.nullable; goto _LL975;
_LL975: _temp974= _temp970.tq; goto _LL973; _LL973: _temp972= _temp970.bounds;
goto _LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp976)){ _temp962= Cyc_Absyn_star_typ;}
goto _LL965; _LL969: goto _LL965; _LL965:;} return _temp962(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v,( void*) Cyc_Absyn_HeapRgn, _temp923);}
_LL951: goto _LL945; _LL945:;}{ void* _temp984; int _temp986; struct _tuple6
_temp982= Cyc_Tcutil_addressof_props( te, e); _LL987: _temp986= _temp982.f1;
goto _LL985; _LL985: _temp984= _temp982.f2; goto _LL983; _LL983: { struct Cyc_Absyn_Tqual
tq= Cyc_Absyn_empty_tqual(); if( _temp986){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, _temp984, tq); return
t;}}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp988= Cyc_Absynpp_typ2string(
t); xprintf("sizeof applied to type %.*s, which has unknown size here",
_get_arr_size( _temp988, 1u), _temp988.curr);}));} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp989= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp997; struct Cyc_List_List* _temp999; struct _tuple1* _temp1001; struct Cyc_List_List*
_temp1003; _LL991: if(( unsigned int) _temp989 > 4u?*(( int*) _temp989) == Cyc_Absyn_StructType:
0){ _LL1002: _temp1001=(( struct Cyc_Absyn_StructType_struct*) _temp989)->f1;
goto _LL1000; _LL1000: _temp999=(( struct Cyc_Absyn_StructType_struct*) _temp989)->f2;
goto _LL998; _LL998: _temp997=(( struct Cyc_Absyn_StructType_struct*) _temp989)->f3;
goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int) _temp989 > 4u?*((
int*) _temp989) == Cyc_Absyn_AnonStructType: 0){ _LL1004: _temp1003=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp989)->f1; goto _LL994;} else{ goto _LL995;}
_LL995: goto _LL996; _LL992: if( _temp997 == 0){ return(( void*(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u));} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp997)))->fields == 0){ goto _LL990;} goto _LL994; _LL994:
bad_type= 0; goto _LL990; _LL996: goto _LL990; _LL990:;} if( bad_type){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp1005= Cyc_Absynpp_typ2string(
t); xprintf("%.*s is not a known struct type", _get_arr_size( _temp1005, 1u),
_temp1005.curr);}));} return Cyc_Absyn_uint_t;}} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1006= t;
struct Cyc_Absyn_PtrInfo _temp1012; struct Cyc_Absyn_Conref* _temp1014; struct
Cyc_Absyn_Tqual _temp1016; struct Cyc_Absyn_Conref* _temp1018; void* _temp1020;
void* _temp1022; _LL1008: if(( unsigned int) _temp1006 > 4u?*(( int*) _temp1006)
== Cyc_Absyn_PointerType: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_PointerType_struct*)
_temp1006)->f1; _LL1023: _temp1022=( void*) _temp1012.elt_typ; goto _LL1021;
_LL1021: _temp1020=( void*) _temp1012.rgn_typ; goto _LL1019; _LL1019: _temp1018=
_temp1012.nullable; goto _LL1017; _LL1017: _temp1016= _temp1012.tq; goto _LL1015;
_LL1015: _temp1014= _temp1012.bounds; goto _LL1009;} else{ goto _LL1010;}
_LL1010: goto _LL1011; _LL1009: Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1020); Cyc_Tcutil_check_nonzero_bound( loc, _temp1014); return _temp1022;
_LL1011: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1024= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_get_arr_size( _temp1024, 1u), _temp1024.curr);})); _LL1007:;}} static void* Cyc_Tcexp_tcStructMember(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1025= t; struct Cyc_Absyn_Structdecl**
_temp1041; struct Cyc_Absyn_Structdecl* _temp1043; struct Cyc_List_List*
_temp1044; struct _tuple1* _temp1046; struct Cyc_List_List* _temp1048; struct
Cyc_List_List* _temp1050; struct Cyc_Absyn_Uniondecl** _temp1052; struct Cyc_Absyn_Uniondecl*
_temp1054; struct Cyc_List_List* _temp1055; struct _tuple1* _temp1057; struct
Cyc_Absyn_Exp* _temp1060; struct Cyc_Absyn_Tqual _temp1062; void* _temp1064;
struct Cyc_Absyn_PtrInfo _temp1067; _LL1027: if(( unsigned int) _temp1025 > 4u?*((
int*) _temp1025) == Cyc_Absyn_StructType: 0){ _LL1047: _temp1046=(( struct Cyc_Absyn_StructType_struct*)
_temp1025)->f1; goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_StructType_struct*)
_temp1025)->f2; goto _LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_StructType_struct*)
_temp1025)->f3; if( _temp1041 == 0){ goto _LL1029;} else{ _temp1043=* _temp1041;
goto _LL1028;}} else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp1025 > 4u?*((
int*) _temp1025) == Cyc_Absyn_AnonStructType: 0){ _LL1049: _temp1048=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1025)->f1; goto _LL1030;} else{ goto
_LL1031;} _LL1031: if(( unsigned int) _temp1025 > 4u?*(( int*) _temp1025) == Cyc_Absyn_AnonUnionType:
0){ _LL1051: _temp1050=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1025)->f1;
goto _LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp1025 > 4u?*((
int*) _temp1025) == Cyc_Absyn_UnionType: 0){ _LL1058: _temp1057=(( struct Cyc_Absyn_UnionType_struct*)
_temp1025)->f1; goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_UnionType_struct*)
_temp1025)->f2; goto _LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_UnionType_struct*)
_temp1025)->f3; if( _temp1052 == 0){ goto _LL1035;} else{ _temp1054=* _temp1052;
goto _LL1034;}} else{ goto _LL1035;} _LL1035: if(( unsigned int) _temp1025 > 4u?*((
int*) _temp1025) == Cyc_Absyn_ArrayType: 0){ _LL1065: _temp1064=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1025)->f1; goto _LL1063; _LL1063:
_temp1062=(( struct Cyc_Absyn_ArrayType_struct*) _temp1025)->f2; goto _LL1061;
_LL1061: _temp1060=(( struct Cyc_Absyn_ArrayType_struct*) _temp1025)->f3; goto
_LL1059;} else{ goto _LL1037;} _LL1059: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ goto _LL1036;} else{ goto _LL1037;} _LL1037:
if(( unsigned int) _temp1025 > 4u?*(( int*) _temp1025) == Cyc_Absyn_PointerType:
0){ _LL1068: _temp1067=(( struct Cyc_Absyn_PointerType_struct*) _temp1025)->f1;
goto _LL1066;} else{ goto _LL1039;} _LL1066: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ goto _LL1038;} else{ goto
_LL1039;} _LL1039: goto _LL1040; _LL1028: if( _temp1046 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u));}{ struct Cyc_Absyn_Structfield* _temp1069= Cyc_Absyn_lookup_struct_field(
_temp1043, f); goto _LL1070; _LL1070: if( _temp1069 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1071= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1046)); struct _tagged_arr _temp1072=* f;
xprintf("struct %.*s has no %.*s field", _get_arr_size( _temp1071, 1u),
_temp1071.curr, _get_arr_size( _temp1072, 1u), _temp1072.curr);}));}{ void* t2;{
struct _RegionHandle _temp1073= _new_region(); struct _RegionHandle* rgn=&
_temp1073; _push_region( rgn);{ struct Cyc_List_List* _temp1074=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1043->tvs, _temp1044);
goto _LL1075; _LL1075: t2= Cyc_Tcutil_rsubstitute( rgn, _temp1074,( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1069))->type);}; _pop_region(
rgn);} return t2;}} _LL1030: { struct Cyc_Absyn_Structfield* _temp1076= Cyc_Absyn_lookup_field(
_temp1048, f); goto _LL1077; _LL1077: if( _temp1076 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1078=* f; xprintf("struct has no %.*s field",
_get_arr_size( _temp1078, 1u), _temp1078.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1076))->type;} _LL1032: { struct Cyc_Absyn_Structfield*
_temp1079= Cyc_Absyn_lookup_field( _temp1050, f); goto _LL1080; _LL1080: if(
_temp1079 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr _temp1081=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1081, 1u), _temp1081.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1079))->type;} _LL1034: if( _temp1057 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp1082= Cyc_Absyn_lookup_union_field(
_temp1054, f); goto _LL1083; _LL1083: if( _temp1082 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1084= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1057)); struct _tagged_arr _temp1085=* f;
xprintf("union %.*s has no %.*s field", _get_arr_size( _temp1084, 1u), _temp1084.curr,
_get_arr_size( _temp1085, 1u), _temp1085.curr);}));}{ struct Cyc_List_List*
_temp1086=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1054->tvs, _temp1055); goto _LL1087; _LL1087: { void*
_temp1088= Cyc_Tcutil_substitute( _temp1086,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1082))->type); goto _LL1089; _LL1089: return _temp1088;}}}
_LL1036:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1090=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1090[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1091; _temp1091.tag= Cyc_Absyn_Primop_e;
_temp1091.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1091.f2=({ struct Cyc_List_List*
_temp1092=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1092->hd=( void*) e; _temp1092->tl= 0; _temp1092;}); _temp1091;});
_temp1090;}))); return Cyc_Absyn_uint_t; _LL1038:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1093=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1093[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1094; _temp1094.tag= Cyc_Absyn_Primop_e;
_temp1094.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1094.f2=({ struct Cyc_List_List*
_temp1095=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1095->hd=( void*) e; _temp1095->tl= 0; _temp1095;}); _temp1094;});
_temp1093;}))); return Cyc_Absyn_uint_t; _LL1040: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1096= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct, union or array, found %.*s", _get_arr_size(
_temp1096, 1u), _temp1096.curr);}));} else{ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1097= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct or union, found %.*s", _get_arr_size( _temp1097, 1u),
_temp1097.curr);}));} _LL1026:;}} static void* Cyc_Tcexp_tcStructArrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e, struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1098= t;
struct Cyc_Absyn_PtrInfo _temp1104; struct Cyc_Absyn_Conref* _temp1106; struct
Cyc_Absyn_Tqual _temp1108; struct Cyc_Absyn_Conref* _temp1110; void* _temp1112;
void* _temp1114; _LL1100: if(( unsigned int) _temp1098 > 4u?*(( int*) _temp1098)
== Cyc_Absyn_PointerType: 0){ _LL1105: _temp1104=(( struct Cyc_Absyn_PointerType_struct*)
_temp1098)->f1; _LL1115: _temp1114=( void*) _temp1104.elt_typ; goto _LL1113;
_LL1113: _temp1112=( void*) _temp1104.rgn_typ; goto _LL1111; _LL1111: _temp1110=
_temp1104.nullable; goto _LL1109; _LL1109: _temp1108= _temp1104.tq; goto _LL1107;
_LL1107: _temp1106= _temp1104.bounds; goto _LL1101;} else{ goto _LL1102;}
_LL1102: goto _LL1103; _LL1101: Cyc_Tcutil_check_nonzero_bound( loc, _temp1106);{
void* _temp1116= Cyc_Tcutil_compress( _temp1114); struct Cyc_Absyn_Structdecl**
_temp1128; struct Cyc_Absyn_Structdecl* _temp1130; struct Cyc_List_List*
_temp1131; struct _tuple1* _temp1133; struct Cyc_Absyn_Uniondecl** _temp1135;
struct Cyc_Absyn_Uniondecl* _temp1137; struct Cyc_List_List* _temp1138; struct
_tuple1* _temp1140; struct Cyc_List_List* _temp1142; struct Cyc_List_List*
_temp1144; _LL1118: if(( unsigned int) _temp1116 > 4u?*(( int*) _temp1116) ==
Cyc_Absyn_StructType: 0){ _LL1134: _temp1133=(( struct Cyc_Absyn_StructType_struct*)
_temp1116)->f1; goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_StructType_struct*)
_temp1116)->f2; goto _LL1129; _LL1129: _temp1128=(( struct Cyc_Absyn_StructType_struct*)
_temp1116)->f3; if( _temp1128 == 0){ goto _LL1120;} else{ _temp1130=* _temp1128;
goto _LL1119;}} else{ goto _LL1120;} _LL1120: if(( unsigned int) _temp1116 > 4u?*((
int*) _temp1116) == Cyc_Absyn_UnionType: 0){ _LL1141: _temp1140=(( struct Cyc_Absyn_UnionType_struct*)
_temp1116)->f1; goto _LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_UnionType_struct*)
_temp1116)->f2; goto _LL1136; _LL1136: _temp1135=(( struct Cyc_Absyn_UnionType_struct*)
_temp1116)->f3; if( _temp1135 == 0){ goto _LL1122;} else{ _temp1137=* _temp1135;
goto _LL1121;}} else{ goto _LL1122;} _LL1122: if(( unsigned int) _temp1116 > 4u?*((
int*) _temp1116) == Cyc_Absyn_AnonStructType: 0){ _LL1143: _temp1142=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1116)->f1; goto _LL1123;} else{ goto
_LL1124;} _LL1124: if(( unsigned int) _temp1116 > 4u?*(( int*) _temp1116) == Cyc_Absyn_AnonUnionType:
0){ _LL1145: _temp1144=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1116)->f1;
goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1119: { struct Cyc_Absyn_Structfield*
_temp1146= Cyc_Absyn_lookup_struct_field( _temp1130, f); goto _LL1147; _LL1147:
if( _temp1146 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr _temp1148= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1133)); struct _tagged_arr _temp1149=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp1148, 1u), _temp1148.curr, _get_arr_size( _temp1149, 1u),
_temp1149.curr);}));}{ void* t3;{ struct _RegionHandle _temp1150= _new_region();
struct _RegionHandle* rgn=& _temp1150; _push_region( rgn);{ struct Cyc_List_List*
_temp1151=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1130->tvs, _temp1131); goto _LL1152; _LL1152: t3= Cyc_Tcutil_rsubstitute(
rgn, _temp1151,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1146))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1112); return
t3;}} _LL1121: { struct Cyc_Absyn_Structfield* _temp1153= Cyc_Absyn_lookup_union_field(
_temp1137, f); goto _LL1154; _LL1154: if( _temp1153 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1155= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1140)); struct _tagged_arr _temp1156=* f;
xprintf("union %.*s has no %.*s field", _get_arr_size( _temp1155, 1u), _temp1155.curr,
_get_arr_size( _temp1156, 1u), _temp1156.curr);}));}{ struct Cyc_List_List*
_temp1157=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1137->tvs, _temp1138); goto _LL1158; _LL1158: { void*
_temp1159= Cyc_Tcutil_substitute( _temp1157,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1153))->type); goto _LL1160; _LL1160: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1112); return _temp1159;}}} _LL1123: { struct Cyc_Absyn_Structfield*
_temp1161= Cyc_Absyn_lookup_field( _temp1142, f); goto _LL1162; _LL1162: if(
_temp1161 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr _temp1163=* f; xprintf("struct has no %.*s field",
_get_arr_size( _temp1163, 1u), _temp1163.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1161))->type;} _LL1125: { struct Cyc_Absyn_Structfield*
_temp1164= Cyc_Absyn_lookup_field( _temp1144, f); goto _LL1165; _LL1165: if(
_temp1164 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr _temp1166=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1166, 1u), _temp1166.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1164))->type;} _LL1127: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1167= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1167, 1u),
_temp1167.curr);})); _LL1117:;} _LL1103: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1168= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1168, 1u),
_temp1168.curr);})); _LL1099:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1169; _push_handler(& _temp1169);{ int _temp1171= 0; if(
setjmp( _temp1169.handler)){ _temp1171= 1;} if( ! _temp1171){{ void* _temp1172=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1172;}; _pop_handler();} else{ void* _temp1170=(
void*) _exn_thrown; void* _temp1174= _temp1170; _LL1176: if( _temp1174 == Cyc_List_Nth){
goto _LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr) xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1179:(
void) _throw( _temp1174); _LL1175:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1180= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_get_arr_size( _temp1180, 1u), _temp1180.curr);}));}{ void* _temp1181= t1;
struct Cyc_Absyn_PtrInfo _temp1189; struct Cyc_Absyn_Conref* _temp1191; struct
Cyc_Absyn_Tqual _temp1193; struct Cyc_Absyn_Conref* _temp1195; void* _temp1197;
void* _temp1199; struct Cyc_List_List* _temp1201; _LL1183: if(( unsigned int)
_temp1181 > 4u?*(( int*) _temp1181) == Cyc_Absyn_PointerType: 0){ _LL1190:
_temp1189=(( struct Cyc_Absyn_PointerType_struct*) _temp1181)->f1; _LL1200:
_temp1199=( void*) _temp1189.elt_typ; goto _LL1198; _LL1198: _temp1197=( void*)
_temp1189.rgn_typ; goto _LL1196; _LL1196: _temp1195= _temp1189.nullable; goto
_LL1194; _LL1194: _temp1193= _temp1189.tq; goto _LL1192; _LL1192: _temp1191=
_temp1189.bounds; goto _LL1184;} else{ goto _LL1185;} _LL1185: if(( unsigned int)
_temp1181 > 4u?*(( int*) _temp1181) == Cyc_Absyn_TupleType: 0){ _LL1202:
_temp1201=(( struct Cyc_Absyn_TupleType_struct*) _temp1181)->f1; goto _LL1186;}
else{ goto _LL1187;} _LL1187: goto _LL1188; _LL1184: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1191);} else{{ void* _temp1203=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1191))->v; void*
_temp1209; struct Cyc_Absyn_Exp* _temp1211; struct Cyc_Absyn_Exp _temp1213;
struct Cyc_Position_Segment* _temp1214; void* _temp1216; void* _temp1218; int
_temp1220; void* _temp1222; struct Cyc_Core_Opt* _temp1224; _LL1205: if((
unsigned int) _temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_Eq_constr: 0){
_LL1210: _temp1209=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1203)->f1;
if(( unsigned int) _temp1209 > 1u?*(( int*) _temp1209) == Cyc_Absyn_Upper_b: 0){
_LL1212: _temp1211=(( struct Cyc_Absyn_Upper_b_struct*) _temp1209)->f1;
_temp1213=* _temp1211; _LL1225: _temp1224= _temp1213.topt; goto _LL1217; _LL1217:
_temp1216=( void*) _temp1213.r; if(*(( int*) _temp1216) == Cyc_Absyn_Const_e){
_LL1219: _temp1218=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1216)->f1;
if(( unsigned int) _temp1218 > 1u?*(( int*) _temp1218) == Cyc_Absyn_Int_c: 0){
_LL1223: _temp1222=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1218)->f1;
if( _temp1222 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1221;} else{ goto _LL1207;}
_LL1221: _temp1220=(( struct Cyc_Absyn_Int_c_struct*) _temp1218)->f2; if(
_temp1220 == 1){ goto _LL1215;} else{ goto _LL1207;}} else{ goto _LL1207;}}
else{ goto _LL1207;} _LL1215: _temp1214= _temp1213.loc; goto _LL1206;} else{
goto _LL1207;}} else{ goto _LL1207;} _LL1207: goto _LL1208; _LL1206: Cyc_Tcutil_warn(
e1->loc, _tag_arr("subscript applied to pointer to one object", sizeof(
unsigned char), 43u)); goto _LL1204; _LL1208: goto _LL1204; _LL1204:;} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1191);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1197); return
_temp1199; _LL1186: return Cyc_Tcexp_ithTupleType( te, loc, _temp1201, e2);
_LL1188: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1226= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_get_arr_size( _temp1226, 1u), _temp1226.curr);})); _LL1182:;}}} static void*
Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1227= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1233; _LL1229: if((
unsigned int) _temp1227 > 4u?*(( int*) _temp1227) == Cyc_Absyn_TupleType: 0){
_LL1234: _temp1233=(( struct Cyc_Absyn_TupleType_struct*) _temp1227)->f1; goto
_LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232; _LL1230: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1233) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc, _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u)); goto _LL1228;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1233=(( struct Cyc_List_List*) _check_null(
_temp1233))->tl)){ void* _temp1235=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1233))->hd)).f2; goto _LL1236; _LL1236: Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1235,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1237=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1237->hd=( void*)({ struct
_tuple8* _temp1238=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1238->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1233))->hd)).f1; _temp1238->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1238;}); _temp1237->tl= fields; _temp1237;});} done= 1; goto _LL1228;
_LL1232: goto _LL1228; _LL1228:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1239->hd=( void*)({ struct _tuple8* _temp1240=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1240->f1= Cyc_Absyn_empty_tqual(); _temp1240->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1240;}); _temp1239->tl=
fields; _temp1239;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1241=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1241[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1242; _temp1242.tag= Cyc_Absyn_TupleType;
_temp1242.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1242;}); _temp1241;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1263=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1263->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1263;})); struct Cyc_Absyn_Const_e_struct*
_temp1247=({ struct Cyc_Absyn_Const_e_struct* _temp1243=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1243[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1244; _temp1244.tag= Cyc_Absyn_Const_e; _temp1244.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1245=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1245[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1246; _temp1246.tag= Cyc_Absyn_Int_c; _temp1246.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1246.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1246;}); _temp1245;})); _temp1244;}); _temp1243;}); goto _LL1248; _LL1248: {
struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp(( void*) _temp1247, loc); sz_exp->topt=({
struct Cyc_Core_Opt* _temp1249=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1249->v=( void*) Cyc_Absyn_uint_t; _temp1249;});{ void*
res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1261=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1261[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1262; _temp1262.tag= Cyc_Absyn_ArrayType;
_temp1262.f1=( void*) res; _temp1262.f2= Cyc_Absyn_empty_tqual(); _temp1262.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1262;}); _temp1261;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1250= Cyc_Absynpp_typ2string( res);
xprintf("elements of array do not all have the same type (%.*s)", _get_arr_size(
_temp1250, 1u), _temp1250.curr);}));}{ int offset= 0; for( 0; des != 0;( offset
++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1;
if( ds != 0){ void* _temp1251=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tagged_arr* _temp1257; struct Cyc_Absyn_Exp* _temp1259; _LL1253: if(*((
int*) _temp1251) == Cyc_Absyn_FieldName){ _LL1258: _temp1257=(( struct Cyc_Absyn_FieldName_struct*)
_temp1251)->f1; goto _LL1254;} else{ goto _LL1255;} _LL1255: if(*(( int*)
_temp1251) == Cyc_Absyn_ArrayElement){ _LL1260: _temp1259=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1251)->f1; goto _LL1256;} else{ goto _LL1252;} _LL1254: Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u)); goto _LL1252; _LL1256: Cyc_Tcexp_tcExpInitializer( te, 0,
_temp1259);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1259); if( i !=
offset){ Cyc_Tcutil_terr( _temp1259->loc,( struct _tagged_arr) xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1252;} _LL1252:;}}} return res_t2;}}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1264= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_get_arr_size( _temp1264, 1u), _temp1264.curr);}));} if( !( vd->tq).q_const){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1265= 0; goto _LL1266; _LL1266: {
struct Cyc_Absyn_Tqual* _temp1267= 0; goto _LL1268; _LL1268: if( topt != 0){
void* _temp1269= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct
Cyc_Absyn_PtrInfo _temp1277; struct Cyc_Absyn_Exp* _temp1279; struct Cyc_Absyn_Tqual
_temp1281; struct Cyc_Absyn_Tqual* _temp1283; void* _temp1284; void** _temp1286;
_LL1271: if(( unsigned int) _temp1269 > 4u?*(( int*) _temp1269) == Cyc_Absyn_PointerType:
0){ _LL1278: _temp1277=(( struct Cyc_Absyn_PointerType_struct*) _temp1269)->f1;
goto _LL1272;} else{ goto _LL1273;} _LL1273: if(( unsigned int) _temp1269 > 4u?*((
int*) _temp1269) == Cyc_Absyn_ArrayType: 0){ _LL1285: _temp1284=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f1; _temp1286=&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f1); goto _LL1282; _LL1282:
_temp1281=(( struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f2; _temp1283=&((
struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f2; goto _LL1280; _LL1280:
_temp1279=(( struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f3; goto _LL1274;}
else{ goto _LL1275;} _LL1275: goto _LL1276; _LL1272: pinfo= _temp1277; _temp1265=(
void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1267=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1270; _LL1274: _temp1265=( void**) _temp1286; _temp1267=(
struct Cyc_Absyn_Tqual*) _temp1283; goto _LL1270; _LL1276: goto _LL1270; _LL1270:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1265, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1287=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1287[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1288; _temp1288.tag= Cyc_Absyn_ArrayType;
_temp1288.f1=( void*) t; _temp1288.f2= _temp1267 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1267)); _temp1288.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1288;}); _temp1287;});}}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1289; _push_handler(&
_temp1289);{ int _temp1291= 0; if( setjmp( _temp1289.handler)){ _temp1291= 1;}
if( ! _temp1291){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1290=( void*) _exn_thrown; void* _temp1293= _temp1290; _LL1295:
if( _temp1293 == Cyc_Dict_Absent){ goto _LL1296;} else{ goto _LL1297;} _LL1297:
goto _LL1298; _LL1296: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1299= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_get_arr_size( _temp1299, 1u), _temp1299.curr);})); return topt != 0?*(( void**)
_check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1298:( void) _throw(
_temp1293); _LL1294:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1300= _new_region(); struct _RegionHandle* rgn=&
_temp1300; _push_region( rgn);{ struct _tuple4 _temp1302=({ struct _tuple4
_temp1301; _temp1301.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1301.f2= rgn;
_temp1301;}); goto _LL1303; _LL1303: { struct Cyc_List_List* _temp1304=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1302, sd->tvs); goto _LL1305; _LL1305: {
struct Cyc_List_List* _temp1306=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1304); goto _LL1307; _LL1307: { struct Cyc_Absyn_StructType_struct*
_temp1311=({ struct Cyc_Absyn_StructType_struct* _temp1308=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1308[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1309; _temp1309.tag= Cyc_Absyn_StructType;
_temp1309.f1=( struct _tuple1*)* tn; _temp1309.f2= _temp1306; _temp1309.f3=({
struct Cyc_Absyn_Structdecl** _temp1310=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1310[ 0]= sd; _temp1310;});
_temp1309;}); _temp1308;}); goto _LL1312; _LL1312: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1311,*(( void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt*
_temp1313=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1313->v=( void*) _temp1306; _temp1313;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u));{
void* _temp1314=( void*) _temp1311; _npop_handler( 0u); return _temp1314;}}{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, loc, args,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd->fields))->v); for( 0; fields != 0; fields=((
struct Cyc_List_List*) _check_null( fields))->tl){ struct _tuple9 _temp1317;
struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Structfield* _temp1320; struct
_tuple9* _temp1315=( struct _tuple9*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1317=* _temp1315; _LL1321: _temp1320= _temp1317.f1; goto
_LL1319; _LL1319: _temp1318= _temp1317.f2; goto _LL1316; _LL1316: { void*
_temp1322= Cyc_Tcutil_rsubstitute( rgn, _temp1304,( void*) _temp1320->type);
goto _LL1323; _LL1323: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1322,
_temp1318); if( ! Cyc_Tcutil_coerce_arg( te, _temp1318, _temp1322)){ Cyc_Tcutil_terr(
_temp1318->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1324=* _temp1320->name;
struct _tagged_arr _temp1325= Cyc_Absynpp_qvar2string(* tn); struct _tagged_arr
_temp1326= Cyc_Absynpp_typ2string( _temp1322); struct _tagged_arr _temp1327= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1318->topt))->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_get_arr_size( _temp1324, 1u), _temp1324.curr, _get_arr_size( _temp1325, 1u),
_temp1325.curr, _get_arr_size( _temp1326, 1u), _temp1326.curr, _get_arr_size(
_temp1327, 1u), _temp1327.curr);}));}}}{ void* _temp1328=( void*) _temp1311;
_npop_handler( 0u); return _temp1328;}}}}}}; _pop_region( rgn);}} static void*
Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* ts, struct Cyc_List_List* args){{ struct _RegionHandle _temp1329=
_new_region(); struct _RegionHandle* rgn=& _temp1329; _push_region( rgn);{ void*
_temp1330= Cyc_Tcutil_compress( ts); struct Cyc_List_List* _temp1336; _LL1332:
if(( unsigned int) _temp1330 > 4u?*(( int*) _temp1330) == Cyc_Absyn_AnonStructType:
0){ _LL1337: _temp1336=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1330)->f1;
goto _LL1333;} else{ goto _LL1334;} _LL1334: goto _LL1335; _LL1333: { struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1336); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1340; struct Cyc_Absyn_Exp*
_temp1341; struct Cyc_Absyn_Structfield* _temp1343; struct _tuple9* _temp1338=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1340=*
_temp1338; _LL1344: _temp1343= _temp1340.f1; goto _LL1342; _LL1342: _temp1341=
_temp1340.f2; goto _LL1339; _LL1339: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1343->type)), _temp1341); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1341,( void*) _temp1343->type)){ Cyc_Tcutil_terr( _temp1341->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1345=* _temp1343->name; struct
_tagged_arr _temp1346= Cyc_Absynpp_typ2string(( void*) _temp1343->type); struct
_tagged_arr _temp1347= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1341->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_get_arr_size( _temp1345, 1u), _temp1345.curr, _get_arr_size( _temp1346, 1u),
_temp1346.curr, _get_arr_size( _temp1347, 1u), _temp1347.curr);}));}} goto
_LL1331;} _LL1335:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1331; _LL1331:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1348=
_new_region(); struct _RegionHandle* rgn=& _temp1348; _push_region( rgn);{
struct _tuple4 _temp1350=({ struct _tuple4 _temp1349; _temp1349.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1349.f2= rgn; _temp1349;}); goto _LL1351; _LL1351: { struct Cyc_List_List*
_temp1352=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1350, tud->tvs);
goto _LL1353; _LL1353: { struct Cyc_List_List* _temp1354=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1350, tuf->tvs); goto _LL1355; _LL1355: { struct Cyc_List_List* _temp1356=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1352); goto
_LL1357; _LL1357: { struct Cyc_List_List* _temp1358=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1354); goto _LL1359; _LL1359: { struct Cyc_List_List*
_temp1360=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1352, _temp1354); goto
_LL1361; _LL1361:* all_ref=({ struct Cyc_Core_Opt* _temp1362=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1362->v=( void*) _temp1356;
_temp1362;});* exist_ref=({ struct Cyc_Core_Opt* _temp1363=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1363->v=( void*) _temp1358;
_temp1363;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1397=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1397[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1398; _temp1398.tag= Cyc_Absyn_TunionFieldType; _temp1398.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1399; _temp1399.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1400=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1400[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1401; _temp1401.tag= Cyc_Absyn_KnownTunionfield; _temp1401.f1= tud;
_temp1401.f2= tuf; _temp1401;}); _temp1400;})); _temp1399.targs= _temp1356;
_temp1399;}); _temp1398;}); _temp1397;}); if( topt != 0){ void* _temp1364= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo _temp1372; struct
Cyc_Absyn_TunionInfo _temp1374; void* _temp1376; struct Cyc_List_List* _temp1378;
void* _temp1380; _LL1366: if(( unsigned int) _temp1364 > 4u?*(( int*) _temp1364)
== Cyc_Absyn_TunionFieldType: 0){ _LL1373: _temp1372=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1364)->f1; goto _LL1367;} else{ goto _LL1368;} _LL1368: if(( unsigned int)
_temp1364 > 4u?*(( int*) _temp1364) == Cyc_Absyn_TunionType: 0){ _LL1375:
_temp1374=(( struct Cyc_Absyn_TunionType_struct*) _temp1364)->f1; _LL1381:
_temp1380=( void*) _temp1374.tunion_info; goto _LL1379; _LL1379: _temp1378=
_temp1374.targs; goto _LL1377; _LL1377: _temp1376=( void*) _temp1374.rgn; goto
_LL1369;} else{ goto _LL1370;} _LL1370: goto _LL1371; _LL1367: Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res); goto _LL1365; _LL1369:{ struct Cyc_List_List*
a= _temp1356; for( 0; a != 0? _temp1378 != 0: 0;( a=(( struct Cyc_List_List*)
_check_null( a))->tl, _temp1378=(( struct Cyc_List_List*) _check_null( _temp1378))->tl)){
Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( _temp1378))->hd);}} if( tuf->typs == 0? es
== 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1382=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1382->v=( void*) res; _temp1382;});
res=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp1383=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1383[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1384; _temp1384.tag= Cyc_Absyn_TunionType;
_temp1384.f1=({ struct Cyc_Absyn_TunionInfo _temp1385; _temp1385.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1386=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1386[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1387; _temp1387.tag= Cyc_Absyn_KnownTunion;
_temp1387.f1= tud; _temp1387;}); _temp1386;})); _temp1385.targs= _temp1356;
_temp1385.rgn=( void*) _temp1376; _temp1385;}); _temp1384;}); _temp1383;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1388= res; _npop_handler( 0u); return _temp1388;}} goto
_LL1365; _LL1371: goto _LL1365; _LL1365:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1360,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1389= Cyc_Absynpp_qvar2string( tuf->name);
struct _tagged_arr _temp1390= Cyc_Absynpp_typ2string( t); struct _tagged_arr
_temp1391= e->topt == 0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_get_arr_size( _temp1389, 1u), _temp1389.curr, _get_arr_size( _temp1390, 1u),
_temp1390.curr, _get_arr_size( _temp1391, 1u), _temp1391.curr);}));}} if( es !=
0){ void* _temp1393= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1392= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp1392, 1u), _temp1392.curr);})); _npop_handler( 0u); return
_temp1393;} if( ts != 0){ void* _temp1395= Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1394= Cyc_Absynpp_qvar2string( tuf->name);
xprintf("too few arguments for tunion constructor %.*s", _get_arr_size(
_temp1394, 1u), _temp1394.curr);})); _npop_handler( 0u); return _temp1395;}{
void* _temp1396= res; _npop_handler( 0u); return _temp1396;}}}}}}}}};
_pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt, void*
t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1411=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1411[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1412; _temp1412.tag= Cyc_Absyn_RgnHandleType;
_temp1412.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1413=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1413->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1413;}));
_temp1412;}); _temp1411;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1402= Cyc_Tcutil_compress(
handle_type); void* _temp1408; _LL1404: if(( unsigned int) _temp1402 > 4u?*((
int*) _temp1402) == Cyc_Absyn_RgnHandleType: 0){ _LL1409: _temp1408=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1402)->f1; goto _LL1405;} else{
goto _LL1406;} _LL1406: goto _LL1407; _LL1405: rgn= _temp1408; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1403; _LL1407: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1410=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_get_arr_size( _temp1410, 1u), _temp1410.curr);})); goto _LL1403; _LL1403:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1414= t; struct Cyc_Absyn_TunionFieldInfo _temp1420; struct Cyc_List_List*
_temp1422; void* _temp1424; struct Cyc_Absyn_Tunionfield* _temp1426; struct Cyc_Absyn_Tuniondecl*
_temp1428; _LL1416: if(( unsigned int) _temp1414 > 4u?*(( int*) _temp1414) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1421: _temp1420=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1414)->f1; _LL1425: _temp1424=( void*) _temp1420.field_info; if(*(( int*)
_temp1424) == Cyc_Absyn_KnownTunionfield){ _LL1429: _temp1428=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1424)->f1; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1424)->f2; goto _LL1423;} else{ goto _LL1418;} _LL1423: _temp1422=
_temp1420.targs; goto _LL1417;} else{ goto _LL1418;} _LL1418: goto _LL1419;
_LL1417: if( _temp1426->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1415; _LL1419: goto _LL1415; _LL1415:;}{
void*(* _temp1430)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
goto _LL1431; _LL1431: if( topt != 0){ void* _temp1432= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1438; struct Cyc_Absyn_Conref*
_temp1440; struct Cyc_Absyn_Tqual _temp1442; struct Cyc_Absyn_Conref* _temp1444;
void* _temp1446; void* _temp1448; _LL1434: if(( unsigned int) _temp1432 > 4u?*((
int*) _temp1432) == Cyc_Absyn_PointerType: 0){ _LL1439: _temp1438=(( struct Cyc_Absyn_PointerType_struct*)
_temp1432)->f1; _LL1449: _temp1448=( void*) _temp1438.elt_typ; goto _LL1447;
_LL1447: _temp1446=( void*) _temp1438.rgn_typ; goto _LL1445; _LL1445: _temp1444=
_temp1438.nullable; goto _LL1443; _LL1443: _temp1442= _temp1438.tq; goto _LL1441;
_LL1441: _temp1440= _temp1438.bounds; goto _LL1435;} else{ goto _LL1436;}
_LL1436: goto _LL1437; _LL1435: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1444)){ _temp1430= Cyc_Absyn_star_typ;} goto _LL1433; _LL1437: goto _LL1433;
_LL1433:;} return _temp1430( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1450=( void*) s->r; struct Cyc_Absyn_Exp* _temp1460;
struct Cyc_Absyn_Stmt* _temp1462; struct Cyc_Absyn_Stmt* _temp1464; struct Cyc_Absyn_Stmt*
_temp1466; struct Cyc_Absyn_Decl* _temp1468; _LL1452: if(( unsigned int)
_temp1450 > 1u?*(( int*) _temp1450) == Cyc_Absyn_Exp_s: 0){ _LL1461: _temp1460=((
struct Cyc_Absyn_Exp_s_struct*) _temp1450)->f1; goto _LL1453;} else{ goto
_LL1454;} _LL1454: if(( unsigned int) _temp1450 > 1u?*(( int*) _temp1450) == Cyc_Absyn_Seq_s:
0){ _LL1465: _temp1464=(( struct Cyc_Absyn_Seq_s_struct*) _temp1450)->f1; goto
_LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_Seq_s_struct*) _temp1450)->f2;
goto _LL1455;} else{ goto _LL1456;} _LL1456: if(( unsigned int) _temp1450 > 1u?*((
int*) _temp1450) == Cyc_Absyn_Decl_s: 0){ _LL1469: _temp1468=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1450)->f1; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1450)->f2; goto _LL1457;} else{ goto _LL1458;} _LL1458: goto _LL1459;
_LL1453: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1460->topt))->v;
_LL1455: s= _temp1462; continue; _LL1457: s= _temp1466; continue; _LL1459:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1451:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1479=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1479[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1480; _temp1480.tag= Cyc_Absyn_RgnHandleType; _temp1480.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1481=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1481->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1481;})); _temp1480;}); _temp1479;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1470= Cyc_Tcutil_compress( handle_type);
void* _temp1476; _LL1472: if(( unsigned int) _temp1470 > 4u?*(( int*) _temp1470)
== Cyc_Absyn_RgnHandleType: 0){ _LL1477: _temp1476=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1470)->f1; goto _LL1473;} else{ goto _LL1474;} _LL1474: goto _LL1475;
_LL1473: rgn= _temp1476; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1471; _LL1475: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1478= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1478, 1u), _temp1478.curr);})); goto _LL1471; _LL1471:;}{ void* _temp1482=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1494; struct Cyc_Absyn_Exp* _temp1496;
struct Cyc_Absyn_Vardecl* _temp1498; struct Cyc_List_List* _temp1500; struct Cyc_Core_Opt*
_temp1502; struct Cyc_List_List* _temp1504; void* _temp1506; struct _tagged_arr
_temp1508; _LL1484: if(*(( int*) _temp1482) == Cyc_Absyn_Comprehension_e){
_LL1499: _temp1498=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1482)->f1;
goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1482)->f2; goto _LL1495; _LL1495: _temp1494=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1482)->f3; goto _LL1485;} else{ goto _LL1486;} _LL1486: if(*(( int*)
_temp1482) == Cyc_Absyn_UnresolvedMem_e){ _LL1503: _temp1502=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1482)->f1; goto _LL1501; _LL1501: _temp1500=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1482)->f2; goto _LL1487;} else{ goto _LL1488;} _LL1488: if(*(( int*)
_temp1482) == Cyc_Absyn_Array_e){ _LL1505: _temp1504=(( struct Cyc_Absyn_Array_e_struct*)
_temp1482)->f1; goto _LL1489;} else{ goto _LL1490;} _LL1490: if(*(( int*)
_temp1482) == Cyc_Absyn_Const_e){ _LL1507: _temp1506=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1482)->f1; if(( unsigned int) _temp1506 > 1u?*(( int*) _temp1506) == Cyc_Absyn_String_c:
0){ _LL1509: _temp1508=(( struct Cyc_Absyn_String_c_struct*) _temp1506)->f1;
goto _LL1491;} else{ goto _LL1492;}} else{ goto _LL1492;} _LL1492: goto _LL1493;
_LL1485: { void* _temp1510= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); goto
_LL1511; _LL1511: { void* _temp1512= Cyc_Tcutil_compress( _temp1510); struct Cyc_Absyn_Exp*
_temp1518; struct Cyc_Absyn_Tqual _temp1520; void* _temp1522; _LL1514: if((
unsigned int) _temp1512 > 4u?*(( int*) _temp1512) == Cyc_Absyn_ArrayType: 0){
_LL1523: _temp1522=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1512)->f1;
goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1512)->f2; goto _LL1519; _LL1519: _temp1518=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1512)->f3; goto _LL1515;} else{ goto _LL1516;} _LL1516: goto _LL1517;
_LL1515: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1496)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1528=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1528[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1529; _temp1529.tag= Cyc_Absyn_Upper_b; _temp1529.f1=
_temp1496; _temp1529;}); _temp1528;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1525=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1525[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1526; _temp1526.tag= Cyc_Absyn_PointerType;
_temp1526.f1=({ struct Cyc_Absyn_PtrInfo _temp1527; _temp1527.elt_typ=( void*)
_temp1522; _temp1527.rgn_typ=( void*) rgn; _temp1527.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1527.tq= _temp1520; _temp1527.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1527;});
_temp1526;}); _temp1525;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1524=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1524->v=(
void*) res_typ; _temp1524;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1517: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1513:;}} _LL1487:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1530=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1530[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1531; _temp1531.tag= Cyc_Absyn_Array_e; _temp1531.f1=
_temp1500; _temp1531;}); _temp1530;}))); _temp1504= _temp1500; goto _LL1489;
_LL1489: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1532= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1538; struct Cyc_Absyn_Conref*
_temp1540; struct Cyc_Absyn_Tqual _temp1542; struct Cyc_Absyn_Conref* _temp1544;
void* _temp1546; void* _temp1548; void** _temp1550; _LL1534: if(( unsigned int)
_temp1532 > 4u?*(( int*) _temp1532) == Cyc_Absyn_PointerType: 0){ _LL1539:
_temp1538=(( struct Cyc_Absyn_PointerType_struct*) _temp1532)->f1; _LL1549:
_temp1548=( void*) _temp1538.elt_typ; _temp1550=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1532)->f1).elt_typ; goto _LL1547; _LL1547: _temp1546=( void*) _temp1538.rgn_typ;
goto _LL1545; _LL1545: _temp1544= _temp1538.nullable; goto _LL1543; _LL1543:
_temp1542= _temp1538.tq; goto _LL1541; _LL1541: _temp1540= _temp1538.bounds;
goto _LL1535;} else{ goto _LL1536;} _LL1536: goto _LL1537; _LL1535: elt_typ_opt=(
void**) _temp1550; goto _LL1533; _LL1537: goto _LL1533; _LL1533:;}{ void*
_temp1551= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); goto _LL1552; _LL1552: {
void* res_typ;{ void* _temp1553= Cyc_Tcutil_compress( _temp1551); struct Cyc_Absyn_Exp*
_temp1559; struct Cyc_Absyn_Tqual _temp1561; void* _temp1563; _LL1555: if((
unsigned int) _temp1553 > 4u?*(( int*) _temp1553) == Cyc_Absyn_ArrayType: 0){
_LL1564: _temp1563=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1553)->f1;
goto _LL1562; _LL1562: _temp1561=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1553)->f2; goto _LL1560; _LL1560: _temp1559=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1553)->f3; goto _LL1556;} else{ goto _LL1557;} _LL1557: goto _LL1558;
_LL1556: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1565=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1565[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1566; _temp1566.tag=
Cyc_Absyn_PointerType; _temp1566.f1=({ struct Cyc_Absyn_PtrInfo _temp1567;
_temp1567.elt_typ=( void*) _temp1563; _temp1567.rgn_typ=( void*) rgn; _temp1567.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1567.tq=
_temp1561; _temp1567.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1568=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1568[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1569; _temp1569.tag= Cyc_Absyn_Upper_b; _temp1569.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1559); _temp1569;}); _temp1568;}));
_temp1567;}); _temp1566;}); _temp1565;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1570=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1570->v=( void*) res_typ; _temp1570;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1554; _LL1558: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcExpNoPromote on Array_e returned non-array type", sizeof(
unsigned char), 50u)); _LL1554:;} return res_typ;}}} _LL1491: { void* _temp1571=
Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
goto _LL1572; _LL1572: { void* _temp1573= Cyc_Tcexp_tcExp( te,( void**)&
_temp1571, e1); goto _LL1574; _LL1574: return Cyc_Absyn_atb_typ( _temp1573, rgn,
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1575=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1575[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1576; _temp1576.tag= Cyc_Absyn_Upper_b;
_temp1576.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1576;}); _temp1575;}));}} _LL1493: {
void** topt2= 0; if( topt != 0){ void* _temp1577= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1585; struct Cyc_Absyn_Conref*
_temp1587; struct Cyc_Absyn_Tqual _temp1589; struct Cyc_Absyn_Conref* _temp1591;
void* _temp1593; void* _temp1595; void** _temp1597; struct Cyc_Absyn_TunionInfo
_temp1598; _LL1579: if(( unsigned int) _temp1577 > 4u?*(( int*) _temp1577) ==
Cyc_Absyn_PointerType: 0){ _LL1586: _temp1585=(( struct Cyc_Absyn_PointerType_struct*)
_temp1577)->f1; _LL1596: _temp1595=( void*) _temp1585.elt_typ; _temp1597=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1577)->f1).elt_typ; goto _LL1594;
_LL1594: _temp1593=( void*) _temp1585.rgn_typ; goto _LL1592; _LL1592: _temp1591=
_temp1585.nullable; goto _LL1590; _LL1590: _temp1589= _temp1585.tq; goto _LL1588;
_LL1588: _temp1587= _temp1585.bounds; goto _LL1580;} else{ goto _LL1581;}
_LL1581: if(( unsigned int) _temp1577 > 4u?*(( int*) _temp1577) == Cyc_Absyn_TunionType:
0){ _LL1599: _temp1598=(( struct Cyc_Absyn_TunionType_struct*) _temp1577)->f1;
goto _LL1582;} else{ goto _LL1583;} _LL1583: goto _LL1584; _LL1580: topt2=( void**)
_temp1597; goto _LL1578; _LL1582: topt2=({ void** _temp1600=( void**) GC_malloc(
sizeof( void*)); _temp1600[ 0]=*(( void**) _check_null( topt)); _temp1600;});
goto _LL1578; _LL1584: goto _LL1578; _LL1578:;}{ void* _temp1601= Cyc_Tcexp_tcExp(
te, topt2, e1); goto _LL1602; _LL1602: { void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1604=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1604[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1605; _temp1605.tag=
Cyc_Absyn_PointerType; _temp1605.f1=({ struct Cyc_Absyn_PtrInfo _temp1606;
_temp1606.elt_typ=( void*) _temp1601; _temp1606.rgn_typ=( void*) rgn; _temp1606.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1606.tq= Cyc_Absyn_empty_tqual();
_temp1606.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1607=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1607[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1608; _temp1608.tag= Cyc_Absyn_Upper_b; _temp1608.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1608;}); _temp1607;})); _temp1606;}); _temp1605;});
_temp1604;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1603=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1603->v=( void*) res_typ;
_temp1603;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}}} _LL1483:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1609= t; struct Cyc_Absyn_Exp* _temp1615; struct Cyc_Absyn_Tqual
_temp1617; void* _temp1619; _LL1611: if(( unsigned int) _temp1609 > 4u?*(( int*)
_temp1609) == Cyc_Absyn_ArrayType: 0){ _LL1620: _temp1619=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f1; goto _LL1618; _LL1618: _temp1617=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f2; goto _LL1616; _LL1616: _temp1615=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f3; goto _LL1612;} else{ goto _LL1613;} _LL1613: goto _LL1614;
_LL1612: { void* _temp1623; int _temp1625; struct _tuple6 _temp1621= Cyc_Tcutil_addressof_props(
te, e); _LL1626: _temp1625= _temp1621.f1; goto _LL1624; _LL1624: _temp1623=
_temp1621.f2; goto _LL1622; _LL1622: { void* _temp1629= _temp1615 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1627=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1627[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1628; _temp1628.tag= Cyc_Absyn_Upper_b;
_temp1628.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1615); _temp1628;});
_temp1627;}); goto _LL1630; _LL1630: t= Cyc_Absyn_atb_typ( _temp1619, _temp1623,
_temp1617, _temp1629);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=(
void*) t); return t;}} _LL1614: return t; _LL1610:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1631=( void*) e->r; struct Cyc_List_List* _temp1641;
struct Cyc_Absyn_Exp* _temp1643; struct Cyc_Absyn_Exp* _temp1645; struct Cyc_Absyn_Vardecl*
_temp1647; void* _temp1649; struct _tagged_arr _temp1651; _LL1633: if(*(( int*)
_temp1631) == Cyc_Absyn_Array_e){ _LL1642: _temp1641=(( struct Cyc_Absyn_Array_e_struct*)
_temp1631)->f1; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(*(( int*)
_temp1631) == Cyc_Absyn_Comprehension_e){ _LL1648: _temp1647=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1631)->f1; goto _LL1646; _LL1646: _temp1645=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1631)->f2; goto _LL1644; _LL1644: _temp1643=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1631)->f3; goto _LL1636;} else{ goto _LL1637;} _LL1637: if(*(( int*)
_temp1631) == Cyc_Absyn_Const_e){ _LL1650: _temp1649=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1631)->f1; if(( unsigned int) _temp1649 > 1u?*(( int*) _temp1649) == Cyc_Absyn_String_c:
0){ _LL1652: _temp1651=(( struct Cyc_Absyn_String_c_struct*) _temp1649)->f1;
goto _LL1638;} else{ goto _LL1639;}} else{ goto _LL1639;} _LL1639: goto _LL1640;
_LL1634: return t; _LL1636: return t; _LL1638: return t; _LL1640: t= Cyc_Tcutil_compress(
t);{ void* _temp1653= t; struct Cyc_Absyn_Exp* _temp1659; struct Cyc_Absyn_Tqual
_temp1661; void* _temp1663; _LL1655: if(( unsigned int) _temp1653 > 4u?*(( int*)
_temp1653) == Cyc_Absyn_ArrayType: 0){ _LL1664: _temp1663=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1653)->f1; goto _LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1653)->f2; goto _LL1660; _LL1660: _temp1659=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1653)->f3; goto _LL1656;} else{ goto _LL1657;} _LL1657: goto _LL1658;
_LL1656: { void* _temp1667; int _temp1669; struct _tuple6 _temp1665= Cyc_Tcutil_addressof_props(
te, e); _LL1670: _temp1669= _temp1665.f1; goto _LL1668; _LL1668: _temp1667=
_temp1665.f2; goto _LL1666; _LL1666: { void* b= _temp1659 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1671=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1671[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1672; _temp1672.tag= Cyc_Absyn_Upper_b; _temp1672.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1659); _temp1672;}); _temp1671;}); t=
Cyc_Absyn_atb_typ( _temp1663, _temp1667, _temp1661, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1658: return t; _LL1654:;} _LL1632:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1673=( void*) e->r; struct Cyc_Absyn_Exp* _temp1679; _LL1675: if(*(( int*)
_temp1673) == Cyc_Absyn_NoInstantiate_e){ _LL1680: _temp1679=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1673)->f1; goto _LL1676;} else{ goto _LL1677;} _LL1677: goto _LL1678;
_LL1676: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1679);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1679->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1679->topt))->v))); e->topt=
_temp1679->topt; goto _LL1674; _LL1678: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1681= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1687; struct Cyc_Absyn_Conref*
_temp1689; struct Cyc_Absyn_Tqual _temp1691; struct Cyc_Absyn_Conref* _temp1693;
void* _temp1695; void* _temp1697; _LL1683: if(( unsigned int) _temp1681 > 4u?*((
int*) _temp1681) == Cyc_Absyn_PointerType: 0){ _LL1688: _temp1687=(( struct Cyc_Absyn_PointerType_struct*)
_temp1681)->f1; _LL1698: _temp1697=( void*) _temp1687.elt_typ; goto _LL1696;
_LL1696: _temp1695=( void*) _temp1687.rgn_typ; goto _LL1694; _LL1694: _temp1693=
_temp1687.nullable; goto _LL1692; _LL1692: _temp1691= _temp1687.tq; goto _LL1690;
_LL1690: _temp1689= _temp1687.bounds; goto _LL1684;} else{ goto _LL1685;}
_LL1685: goto _LL1686; _LL1684:{ void* _temp1699= Cyc_Tcutil_compress( _temp1697);
struct Cyc_Absyn_FnInfo _temp1705; struct Cyc_List_List* _temp1707; struct Cyc_List_List*
_temp1709; struct Cyc_Absyn_VarargInfo* _temp1711; int _temp1713; struct Cyc_List_List*
_temp1715; void* _temp1717; struct Cyc_Core_Opt* _temp1719; struct Cyc_List_List*
_temp1721; _LL1701: if(( unsigned int) _temp1699 > 4u?*(( int*) _temp1699) ==
Cyc_Absyn_FnType: 0){ _LL1706: _temp1705=(( struct Cyc_Absyn_FnType_struct*)
_temp1699)->f1; _LL1722: _temp1721= _temp1705.tvars; goto _LL1720; _LL1720:
_temp1719= _temp1705.effect; goto _LL1718; _LL1718: _temp1717=( void*) _temp1705.ret_typ;
goto _LL1716; _LL1716: _temp1715= _temp1705.args; goto _LL1714; _LL1714:
_temp1713= _temp1705.c_varargs; goto _LL1712; _LL1712: _temp1711= _temp1705.cyc_varargs;
goto _LL1710; _LL1710: _temp1709= _temp1705.rgn_po; goto _LL1708; _LL1708:
_temp1707= _temp1705.attributes; goto _LL1702;} else{ goto _LL1703;} _LL1703:
goto _LL1704; _LL1702: if( _temp1721 != 0){ struct _RegionHandle _temp1723=
_new_region(); struct _RegionHandle* rgn=& _temp1723; _push_region( rgn);{
struct _tuple4 _temp1725=({ struct _tuple4 _temp1724; _temp1724.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1724.f2= rgn; _temp1724;}); goto _LL1726; _LL1726: { struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)(
struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1725, _temp1721);
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1733=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1733[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1734; _temp1734.tag= Cyc_Absyn_FnType;
_temp1734.f1=({ struct Cyc_Absyn_FnInfo _temp1735; _temp1735.tvars= 0; _temp1735.effect=
_temp1719; _temp1735.ret_typ=( void*) _temp1717; _temp1735.args= _temp1715;
_temp1735.c_varargs= _temp1713; _temp1735.cyc_varargs= _temp1711; _temp1735.rgn_po=
_temp1709; _temp1735.attributes= _temp1707; _temp1735;}); _temp1734;});
_temp1733;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1730=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1730[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1731; _temp1731.tag=
Cyc_Absyn_PointerType; _temp1731.f1=({ struct Cyc_Absyn_PtrInfo _temp1732;
_temp1732.elt_typ=( void*) ftyp; _temp1732.rgn_typ=( void*) _temp1695; _temp1732.nullable=
_temp1693; _temp1732.tq= _temp1691; _temp1732.bounds= _temp1689; _temp1732;});
_temp1731;}); _temp1730;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1727=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1727[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1728; _temp1728.tag= Cyc_Absyn_Instantiate_e; _temp1728.f1= inner;
_temp1728.f2= ts; _temp1728;}); _temp1727;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1729->v=( void*) new_typ; _temp1729;});}}; _pop_region( rgn);} goto _LL1700;
_LL1704: goto _LL1700; _LL1700:;} goto _LL1682; _LL1686: goto _LL1682; _LL1682:;}
goto _LL1674; _LL1674:;} return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc; void* t;{
void* _temp1736=( void*) e->r; struct Cyc_Absyn_Exp* _temp1810; struct _tuple1*
_temp1812; struct Cyc_List_List* _temp1814; struct Cyc_Absyn_Exp* _temp1816;
struct Cyc_List_List* _temp1818; struct Cyc_Core_Opt* _temp1820; void* _temp1822;
void* _temp1824; struct _tuple1* _temp1826; struct Cyc_List_List* _temp1828;
void* _temp1830; void* _temp1832; struct Cyc_Absyn_Exp* _temp1834; struct Cyc_Absyn_Exp*
_temp1836; struct Cyc_Core_Opt* _temp1838; struct Cyc_Absyn_Exp* _temp1840;
struct Cyc_Absyn_Exp* _temp1842; struct Cyc_Absyn_Exp* _temp1844; struct Cyc_Absyn_Exp*
_temp1846; struct Cyc_Absyn_Exp* _temp1848; struct Cyc_Absyn_Exp* _temp1850;
struct Cyc_Absyn_VarargCallInfo* _temp1852; struct Cyc_Absyn_VarargCallInfo**
_temp1854; struct Cyc_List_List* _temp1855; struct Cyc_Absyn_Exp* _temp1857;
struct Cyc_Absyn_Exp* _temp1859; struct Cyc_List_List* _temp1861; struct Cyc_Absyn_Exp*
_temp1863; struct Cyc_Absyn_Exp* _temp1865; void* _temp1867; struct Cyc_Absyn_Exp*
_temp1869; struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Exp* _temp1873;
struct Cyc_Absyn_Exp* _temp1875; void* _temp1877; struct _tagged_arr* _temp1879;
void* _temp1881; struct Cyc_Absyn_Exp* _temp1883; struct _tagged_arr* _temp1885;
struct Cyc_Absyn_Exp* _temp1887; struct _tagged_arr* _temp1889; struct Cyc_Absyn_Exp*
_temp1891; struct Cyc_Absyn_Exp* _temp1893; struct Cyc_Absyn_Exp* _temp1895;
struct Cyc_List_List* _temp1897; struct Cyc_List_List* _temp1899; struct _tuple2*
_temp1901; struct Cyc_List_List* _temp1903; struct Cyc_Absyn_Stmt* _temp1905;
struct Cyc_Absyn_Fndecl* _temp1907; struct Cyc_Absyn_Exp* _temp1909; struct Cyc_Absyn_Exp*
_temp1911; struct Cyc_Absyn_Exp* _temp1913; struct Cyc_Absyn_Vardecl* _temp1915;
struct Cyc_Absyn_Structdecl* _temp1917; struct Cyc_Absyn_Structdecl** _temp1919;
struct Cyc_List_List* _temp1920; struct Cyc_Core_Opt* _temp1922; struct Cyc_Core_Opt**
_temp1924; struct _tuple1* _temp1925; struct _tuple1** _temp1927; struct Cyc_List_List*
_temp1928; void* _temp1930; struct Cyc_Absyn_Tunionfield* _temp1932; struct Cyc_Absyn_Tuniondecl*
_temp1934; struct Cyc_List_List* _temp1936; struct Cyc_Core_Opt* _temp1938;
struct Cyc_Core_Opt** _temp1940; struct Cyc_Core_Opt* _temp1941; struct Cyc_Core_Opt**
_temp1943; struct Cyc_Absyn_Enumfield* _temp1944; struct Cyc_Absyn_Enumdecl*
_temp1946; struct _tuple1* _temp1948; struct _tuple1** _temp1950; void*
_temp1951; struct Cyc_Absyn_Exp* _temp1953; _LL1738: if(*(( int*) _temp1736) ==
Cyc_Absyn_NoInstantiate_e){ _LL1811: _temp1810=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1736)->f1; goto _LL1739;} else{ goto _LL1740;} _LL1740: if(*(( int*)
_temp1736) == Cyc_Absyn_UnknownId_e){ _LL1813: _temp1812=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1736)->f1; goto _LL1741;} else{ goto _LL1742;} _LL1742: if(*(( int*)
_temp1736) == Cyc_Absyn_UnknownCall_e){ _LL1817: _temp1816=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1736)->f1; goto _LL1815; _LL1815: _temp1814=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1736)->f2; goto _LL1743;} else{ goto _LL1744;} _LL1744: if(*(( int*)
_temp1736) == Cyc_Absyn_UnresolvedMem_e){ _LL1821: _temp1820=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1736)->f1; goto _LL1819; _LL1819: _temp1818=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1736)->f2; goto _LL1745;} else{ goto _LL1746;} _LL1746: if(*(( int*)
_temp1736) == Cyc_Absyn_Const_e){ _LL1823: _temp1822=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1736)->f1; goto _LL1747;} else{ goto _LL1748;} _LL1748: if(*(( int*)
_temp1736) == Cyc_Absyn_Var_e){ _LL1827: _temp1826=(( struct Cyc_Absyn_Var_e_struct*)
_temp1736)->f1; goto _LL1825; _LL1825: _temp1824=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1736)->f2; goto _LL1749;} else{ goto _LL1750;} _LL1750: if(*(( int*)
_temp1736) == Cyc_Absyn_Primop_e){ _LL1831: _temp1830=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1736)->f1; goto _LL1829; _LL1829: _temp1828=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1736)->f2; goto _LL1751;} else{ goto _LL1752;} _LL1752: if(*(( int*)
_temp1736) == Cyc_Absyn_Increment_e){ _LL1835: _temp1834=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1736)->f1; goto _LL1833; _LL1833: _temp1832=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1736)->f2; goto _LL1753;} else{ goto _LL1754;} _LL1754: if(*(( int*)
_temp1736) == Cyc_Absyn_AssignOp_e){ _LL1841: _temp1840=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1736)->f1; goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1736)->f2; goto _LL1837; _LL1837: _temp1836=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1736)->f3; goto _LL1755;} else{ goto _LL1756;} _LL1756: if(*(( int*)
_temp1736) == Cyc_Absyn_Conditional_e){ _LL1847: _temp1846=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1736)->f1; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1736)->f2; goto _LL1843; _LL1843: _temp1842=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1736)->f3; goto _LL1757;} else{ goto _LL1758;} _LL1758: if(*(( int*)
_temp1736) == Cyc_Absyn_SeqExp_e){ _LL1851: _temp1850=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1736)->f1; goto _LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1736)->f2; goto _LL1759;} else{ goto _LL1760;} _LL1760: if(*(( int*)
_temp1736) == Cyc_Absyn_FnCall_e){ _LL1858: _temp1857=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1736)->f1; goto _LL1856; _LL1856: _temp1855=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1736)->f2; goto _LL1853; _LL1853: _temp1852=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1736)->f3; _temp1854=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1736)->f3;
goto _LL1761;} else{ goto _LL1762;} _LL1762: if(*(( int*) _temp1736) == Cyc_Absyn_Throw_e){
_LL1860: _temp1859=(( struct Cyc_Absyn_Throw_e_struct*) _temp1736)->f1; goto
_LL1763;} else{ goto _LL1764;} _LL1764: if(*(( int*) _temp1736) == Cyc_Absyn_Instantiate_e){
_LL1864: _temp1863=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1736)->f1;
goto _LL1862; _LL1862: _temp1861=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1736)->f2; goto _LL1765;} else{ goto _LL1766;} _LL1766: if(*(( int*)
_temp1736) == Cyc_Absyn_Cast_e){ _LL1868: _temp1867=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1736)->f1; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1736)->f2; goto _LL1767;} else{ goto _LL1768;} _LL1768: if(*(( int*)
_temp1736) == Cyc_Absyn_Address_e){ _LL1870: _temp1869=(( struct Cyc_Absyn_Address_e_struct*)
_temp1736)->f1; goto _LL1769;} else{ goto _LL1770;} _LL1770: if(*(( int*)
_temp1736) == Cyc_Absyn_New_e){ _LL1874: _temp1873=(( struct Cyc_Absyn_New_e_struct*)
_temp1736)->f1; goto _LL1872; _LL1872: _temp1871=(( struct Cyc_Absyn_New_e_struct*)
_temp1736)->f2; goto _LL1771;} else{ goto _LL1772;} _LL1772: if(*(( int*)
_temp1736) == Cyc_Absyn_Sizeofexp_e){ _LL1876: _temp1875=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1736)->f1; goto _LL1773;} else{ goto _LL1774;} _LL1774: if(*(( int*)
_temp1736) == Cyc_Absyn_Sizeoftyp_e){ _LL1878: _temp1877=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1736)->f1; goto _LL1775;} else{ goto _LL1776;} _LL1776: if(*(( int*)
_temp1736) == Cyc_Absyn_Offsetof_e){ _LL1882: _temp1881=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1736)->f1; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1736)->f2; goto _LL1777;} else{ goto _LL1778;} _LL1778: if(*(( int*)
_temp1736) == Cyc_Absyn_Deref_e){ _LL1884: _temp1883=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1736)->f1; goto _LL1779;} else{ goto _LL1780;} _LL1780: if(*(( int*)
_temp1736) == Cyc_Absyn_StructMember_e){ _LL1888: _temp1887=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1736)->f1; goto _LL1886; _LL1886: _temp1885=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1736)->f2; goto _LL1781;} else{ goto _LL1782;} _LL1782: if(*(( int*)
_temp1736) == Cyc_Absyn_StructArrow_e){ _LL1892: _temp1891=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1736)->f1; goto _LL1890; _LL1890: _temp1889=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1736)->f2; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(*(( int*)
_temp1736) == Cyc_Absyn_Subscript_e){ _LL1896: _temp1895=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1736)->f1; goto _LL1894; _LL1894: _temp1893=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1736)->f2; goto _LL1785;} else{ goto _LL1786;} _LL1786: if(*(( int*)
_temp1736) == Cyc_Absyn_Tuple_e){ _LL1898: _temp1897=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1736)->f1; goto _LL1787;} else{ goto _LL1788;} _LL1788: if(*(( int*)
_temp1736) == Cyc_Absyn_CompoundLit_e){ _LL1902: _temp1901=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1736)->f1; goto _LL1900; _LL1900: _temp1899=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1736)->f2; goto _LL1789;} else{ goto _LL1790;} _LL1790: if(*(( int*)
_temp1736) == Cyc_Absyn_Array_e){ _LL1904: _temp1903=(( struct Cyc_Absyn_Array_e_struct*)
_temp1736)->f1; goto _LL1791;} else{ goto _LL1792;} _LL1792: if(*(( int*)
_temp1736) == Cyc_Absyn_StmtExp_e){ _LL1906: _temp1905=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1736)->f1; goto _LL1793;} else{ goto _LL1794;} _LL1794: if(*(( int*)
_temp1736) == Cyc_Absyn_Codegen_e){ _LL1908: _temp1907=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1736)->f1; goto _LL1795;} else{ goto _LL1796;} _LL1796: if(*(( int*)
_temp1736) == Cyc_Absyn_Fill_e){ _LL1910: _temp1909=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1736)->f1; goto _LL1797;} else{ goto _LL1798;} _LL1798: if(*(( int*)
_temp1736) == Cyc_Absyn_Comprehension_e){ _LL1916: _temp1915=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1736)->f1; goto _LL1914; _LL1914: _temp1913=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1736)->f2; goto _LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1736)->f3; goto _LL1799;} else{ goto _LL1800;} _LL1800: if(*(( int*)
_temp1736) == Cyc_Absyn_Struct_e){ _LL1926: _temp1925=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1736)->f1; _temp1927=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f1;
goto _LL1923; _LL1923: _temp1922=(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f2;
_temp1924=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f2; goto _LL1921;
_LL1921: _temp1920=(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f3; goto
_LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f4;
_temp1919=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1736)->f4; goto _LL1801;}
else{ goto _LL1802;} _LL1802: if(*(( int*) _temp1736) == Cyc_Absyn_AnonStruct_e){
_LL1931: _temp1930=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1736)->f1;
goto _LL1929; _LL1929: _temp1928=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1736)->f2; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(*(( int*)
_temp1736) == Cyc_Absyn_Tunion_e){ _LL1942: _temp1941=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1736)->f1; _temp1943=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f1;
goto _LL1939; _LL1939: _temp1938=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f2;
_temp1940=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f2; goto _LL1937;
_LL1937: _temp1936=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f3; goto
_LL1935; _LL1935: _temp1934=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f4;
goto _LL1933; _LL1933: _temp1932=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1736)->f5;
goto _LL1805;} else{ goto _LL1806;} _LL1806: if(*(( int*) _temp1736) == Cyc_Absyn_Enum_e){
_LL1949: _temp1948=(( struct Cyc_Absyn_Enum_e_struct*) _temp1736)->f1; _temp1950=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1736)->f1; goto _LL1947; _LL1947:
_temp1946=(( struct Cyc_Absyn_Enum_e_struct*) _temp1736)->f2; goto _LL1945;
_LL1945: _temp1944=(( struct Cyc_Absyn_Enum_e_struct*) _temp1736)->f3; goto
_LL1807;} else{ goto _LL1808;} _LL1808: if(*(( int*) _temp1736) == Cyc_Absyn_Malloc_e){
_LL1954: _temp1953=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1736)->f1; goto
_LL1952; _LL1952: _temp1951=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1736)->f2; goto _LL1809;} else{ goto _LL1737;} _LL1739: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1810); return; _LL1741: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1812); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1743: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1816, _temp1814); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1745: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1818); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1747: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1822, e);
goto _LL1737; _LL1749: t= Cyc_Tcexp_tcVar( te, loc, _temp1826, _temp1824); goto
_LL1737; _LL1751: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1830, _temp1828);
goto _LL1737; _LL1753: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1834,
_temp1832); goto _LL1737; _LL1755: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1840, _temp1838, _temp1836); goto _LL1737; _LL1757: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1846, _temp1844, _temp1842); goto _LL1737; _LL1759: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1850, _temp1848); goto _LL1737; _LL1761: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1857, _temp1855, _temp1854); goto _LL1737; _LL1763: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1859); goto _LL1737; _LL1765: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1863, _temp1861); goto _LL1737; _LL1767: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1867, _temp1865); goto _LL1737; _LL1769: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1869); goto _LL1737; _LL1771: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1873, e, _temp1871); goto _LL1737; _LL1773: { void* _temp1955= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1875); goto _LL1956; _LL1956: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1955); goto _LL1737;} _LL1775: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1877); goto _LL1737; _LL1777: t= Cyc_Tcexp_tcOffsetof( te, loc, topt,
_temp1881, _temp1879); goto _LL1737; _LL1779: t= Cyc_Tcexp_tcDeref( te, loc,
topt, _temp1883); goto _LL1737; _LL1781: t= Cyc_Tcexp_tcStructMember( te, loc,
topt, e, _temp1887, _temp1885); goto _LL1737; _LL1783: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1891, _temp1889); goto _LL1737; _LL1785: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1895, _temp1893); goto _LL1737; _LL1787: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1897); goto _LL1737; _LL1789: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1901, _temp1899); goto _LL1737; _LL1791: { void** elt_topt= 0;
if( topt != 0){ void* _temp1957= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1963; struct Cyc_Absyn_Tqual _temp1965; void*
_temp1967; void** _temp1969; _LL1959: if(( unsigned int) _temp1957 > 4u?*(( int*)
_temp1957) == Cyc_Absyn_ArrayType: 0){ _LL1968: _temp1967=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1957)->f1; _temp1969=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1957)->f1); goto _LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1957)->f2; goto _LL1964; _LL1964: _temp1963=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1957)->f3; goto _LL1960;} else{ goto _LL1961;} _LL1961: goto _LL1962;
_LL1960: elt_topt=( void**) _temp1969; goto _LL1958; _LL1962: goto _LL1958;
_LL1958:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1903); goto _LL1737;}
_LL1793: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1905); goto _LL1737;
_LL1795: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1907); goto _LL1737;
_LL1797: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1909); goto _LL1737; _LL1799:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1915, _temp1913, _temp1911);
goto _LL1737; _LL1801: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1927,
_temp1924, _temp1920, _temp1919); goto _LL1737; _LL1803: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1930, _temp1928); goto _LL1737; _LL1805: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1943, _temp1940, _temp1936, _temp1934, _temp1932); goto
_LL1737; _LL1807:* _temp1950=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1944))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1970=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1970[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1971; _temp1971.tag= Cyc_Absyn_EnumType;
_temp1971.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1946))->name;
_temp1971.f2= _temp1946; _temp1971;}); _temp1970;}); goto _LL1737; _LL1809: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1953, _temp1951); goto _LL1737; _LL1737:;}
e->topt=({ struct Cyc_Core_Opt* _temp1972=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1972->v=( void*) t; _temp1972;});}