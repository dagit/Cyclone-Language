 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
21; static const int Cyc_Absyn_Aprintf= 22; static const int Cyc_Absyn_Scanf= 23;
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
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_String_zstrcmp( struct _tagged_arr,
struct _tagged_arr); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_arr); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct
_tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
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
_temp20; struct Cyc_Absyn_Tuniondecl* _temp22; _LL6: if(*(( int*) _temp4) == Cyc_Tcenv_VarRes){
_LL15: _temp14=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp4)->f1; goto
_LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp4) == Cyc_Tcenv_EnumRes){ _LL19:
_temp18=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f1; goto _LL17; _LL17:
_temp16=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f2; goto _LL9;} else{ goto
_LL10;} _LL10: if(*(( int*) _temp4) == Cyc_Tcenv_TunionRes){ _LL23: _temp22=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f1; goto _LL21; _LL21: _temp20=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f2; goto _LL11;} else{ goto _LL12;}
_LL12: if(*(( int*) _temp4) == Cyc_Tcenv_StructRes){ goto _LL13;} else{ goto
_LL5;} _LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp24=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp24[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp25; _temp25.tag= Cyc_Absyn_Var_e;
_temp25.f1= q; _temp25.f2=( void*) _temp14; _temp25;}); _temp24;}))); goto _LL5;
_LL9:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp26=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp27; _temp27.tag= Cyc_Absyn_Enum_e;
_temp27.f1= q; _temp27.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp27.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp27;}); _temp26;}))); goto _LL5; _LL11:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp28=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp29; _temp29.tag= Cyc_Absyn_Tunion_e;
_temp29.f1= 0; _temp29.f2= 0; _temp29.f3= 0; _temp29.f4= _temp22; _temp29.f5=
_temp20; _temp29;}); _temp28;}))); goto _LL5; _LL13: Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp30)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp34= _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp35; _temp35.tag=
Cyc_Stdio_String_pa; _temp35.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{
void* _temp33=( void*)& _temp35; void* _temp31[ 1u]={ _temp33}; struct
_tagged_arr _temp32={( void*) _temp31,( void*) _temp31,( void*)( _temp31 + 1u)};
_temp30( _temp34, _temp32);}}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp36=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp36[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp37; _temp37.tag= Cyc_Absyn_Var_e;
_temp37.f1= q; _temp37.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp37;});
_temp36;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp39= _temp2; _LL41: if( _temp39 == Cyc_Dict_Absent){ goto
_LL42;} else{ goto _LL43;} _LL43: goto _LL44; _LL42:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp45=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp45[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp46; _temp46.tag= Cyc_Absyn_Var_e; _temp46.f1= q; _temp46.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp46;}); _temp45;}))); goto _LL40; _LL44:( void)
_throw( _temp39); _LL40:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp47=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp47->f1= 0; _temp47->f2= e; _temp47;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp48=( void*)
e1->r; struct _tuple1* _temp54; _LL50: if(*(( int*) _temp48) == Cyc_Absyn_UnknownId_e){
_LL55: _temp54=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: goto _LL53; _LL51: { struct _handler_cons _temp56;
_push_handler(& _temp56);{ int _temp58= 0; if( setjmp( _temp56.handler)){
_temp58= 1;} if( ! _temp58){{ void* _temp59= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp54); void* _temp69; struct Cyc_Absyn_Tunionfield* _temp71; struct Cyc_Absyn_Tuniondecl*
_temp73; struct Cyc_Absyn_Structdecl* _temp75; _LL61: if(*(( int*) _temp59) ==
Cyc_Tcenv_VarRes){ _LL70: _temp69=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp59)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp59) ==
Cyc_Tcenv_TunionRes){ _LL74: _temp73=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp59)->f1; goto _LL72; _LL72: _temp71=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp59)->f2; goto _LL64;} else{ goto _LL65;} _LL65: if(*(( int*) _temp59) ==
Cyc_Tcenv_StructRes){ _LL76: _temp75=(( struct Cyc_Tcenv_StructRes_struct*)
_temp59)->f1; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp59) ==
Cyc_Tcenv_EnumRes){ goto _LL68;} else{ goto _LL60;} _LL62:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp77=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp78; _temp78.tag= Cyc_Absyn_FnCall_e; _temp78.f1= e1; _temp78.f2= es;
_temp78.f3= 0; _temp78;}); _temp77;}))); _npop_handler( 0u); return; _LL64: if(
_temp71->typs == 0){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp79)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp83= _tag_arr("%s is a constant, not a function", sizeof(
unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp84; _temp84.tag=
Cyc_Stdio_String_pa; _temp84.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp71->name);{ void* _temp82=( void*)& _temp84; void* _temp80[ 1u]={ _temp82};
struct _tagged_arr _temp81={( void*) _temp80,( void*) _temp80,( void*)( _temp80
+ 1u)}; _temp79( _temp83, _temp81);}}));}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp85=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp85[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp86; _temp86.tag= Cyc_Absyn_Tunion_e; _temp86.f1= 0; _temp86.f2= 0; _temp86.f3=
es; _temp86.f4= _temp73; _temp86.f5= _temp71; _temp86;}); _temp85;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL66: { struct Cyc_List_List*
_temp87=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp75->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp88=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp88[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp89; _temp89.tag= Cyc_Absyn_Struct_e;
_temp89.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp75->name))->v;
_temp89.f2= 0; _temp89.f3= _temp87; _temp89.f4=( struct Cyc_Absyn_Structdecl*)
_temp75; _temp89;}); _temp88;})));} else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name",
sizeof( unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp90=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp90[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp91; _temp91.tag= Cyc_Absyn_Struct_e;
_temp91.f1= _temp54; _temp91.f2= 0; _temp91.f3= _temp87; _temp91.f4=( struct Cyc_Absyn_Structdecl*)
_temp75; _temp91;}); _temp90;})));} _npop_handler( 0u); return;} _LL68: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp92)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp96=
_tag_arr("%s is an enum constructor, not a function", sizeof( unsigned char), 42u);
struct Cyc_Stdio_String_pa_struct _temp97; _temp97.tag= Cyc_Stdio_String_pa;
_temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp54);{ void*
_temp95=( void*)& _temp97; void* _temp93[ 1u]={ _temp95}; struct _tagged_arr
_temp94={( void*) _temp93,( void*) _temp93,( void*)( _temp93 + 1u)}; _temp92(
_temp96, _temp94);}})); _npop_handler( 0u); return; _LL60:;}; _pop_handler();}
else{ void* _temp57=( void*) _exn_thrown; void* _temp99= _temp57; _LL101: if(
_temp99 == Cyc_Dict_Absent){ goto _LL102;} else{ goto _LL103;} _LL103: goto
_LL104; _LL102: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp105)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp109= _tag_arr("unbound identifier %s", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp110; _temp110.tag=
Cyc_Stdio_String_pa; _temp110.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp54);{ void* _temp108=( void*)& _temp110; void* _temp106[ 1u]={ _temp108};
struct _tagged_arr _temp107={( void*) _temp106,( void*) _temp106,( void*)(
_temp106 + 1u)}; _temp105( _temp109, _temp107);}}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp111=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp111[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp112; _temp112.tag= Cyc_Absyn_FnCall_e; _temp112.f1=
e1; _temp112.f2= es; _temp112.f3= 0; _temp112;}); _temp111;}))); return; _LL104:(
void) _throw( _temp99); _LL100:;}}} _LL53:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp113=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp113[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp114; _temp114.tag= Cyc_Absyn_FnCall_e; _temp114.f1=
e1; _temp114.f2= es; _temp114.f3= 0; _temp114;}); _temp113;}))); return; _LL49:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp115=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp115[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp116; _temp116.tag= Cyc_Absyn_Array_e;
_temp116.f1= des; _temp116;}); _temp115;}))); return;}{ void* t=*(( void**)
_check_null( topt)); void* _temp117= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual
_temp127; void* _temp129; struct Cyc_Absyn_Structdecl** _temp131; struct Cyc_List_List*
_temp133; struct _tuple1* _temp135; _LL119: if(( unsigned int) _temp117 > 4u?*((
int*) _temp117) == Cyc_Absyn_ArrayType: 0){ _LL130: _temp129=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp117)->f1; goto _LL128; _LL128: _temp127=((
struct Cyc_Absyn_ArrayType_struct*) _temp117)->f2; goto _LL120;} else{ goto
_LL121;} _LL121: if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_StructType:
0){ _LL136: _temp135=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f1; goto
_LL134; _LL134: _temp133=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f2;
goto _LL132; _LL132: _temp131=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f3;
goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp117 > 4u?*((
int*) _temp117) == Cyc_Absyn_AnonStructType: 0){ goto _LL124;} else{ goto _LL125;}
_LL125: goto _LL126; _LL120:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp137=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp137[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp138; _temp138.tag= Cyc_Absyn_Array_e;
_temp138.f1= des; _temp138;}); _temp137;}))); goto _LL118; _LL122: if( _temp131
== 0? 1: _temp135 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp139=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp139[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp140; _temp140.tag= Cyc_Absyn_Struct_e;
_temp140.f1=( struct _tuple1*) _check_null( _temp135); _temp140.f2= 0; _temp140.f3=
des; _temp140.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp131)); _temp140;}); _temp139;}))); goto _LL118; _LL124:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp141=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp141[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp142; _temp142.tag= Cyc_Absyn_AnonStruct_e;
_temp142.f1=( void*) t; _temp142.f2= des; _temp142;}); _temp141;}))); goto
_LL118; _LL126:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp143=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp143[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp144; _temp144.tag= Cyc_Absyn_Array_e;
_temp144.f1= des; _temp144;}); _temp143;}))); goto _LL118; _LL118:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp145)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp150= _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp152;
_temp152.tag= Cyc_Stdio_String_pa; _temp152.f1=( struct _tagged_arr) msg_part;{
void* _temp148=( void*)& _temp152; struct Cyc_Stdio_String_pa_struct _temp151;
_temp151.tag= Cyc_Stdio_String_pa; _temp151.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp149=(
void*)& _temp151; void* _temp146[ 2u]={ _temp148, _temp149}; struct _tagged_arr
_temp147={( void*) _temp146,( void*) _temp146,( void*)( _temp146 + 2u)};
_temp145( _temp150, _temp147);}}}));}} static void* Cyc_Tcexp_tcConst( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void* c,
struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp153= c; void* _temp169; void*
_temp171; void* _temp173; int _temp175; void* _temp177; struct _tagged_arr
_temp179; _LL155: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_Char_c:
0){ _LL170: _temp169=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp153)->f1;
goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp153 > 1u?*((
int*) _temp153) == Cyc_Absyn_Short_c: 0){ _LL172: _temp171=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp153)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_LongLong_c: 0){ _LL174: _temp173=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp153)->f1; goto _LL160;} else{
goto _LL161;} _LL161: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153) ==
Cyc_Absyn_Float_c: 0){ goto _LL162;} else{ goto _LL163;} _LL163: if((
unsigned int) _temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_Int_c: 0){ _LL178:
_temp177=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp153)->f1; goto _LL176;
_LL176: _temp175=(( struct Cyc_Absyn_Int_c_struct*) _temp153)->f2; goto _LL164;}
else{ goto _LL165;} _LL165: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153)
== Cyc_Absyn_String_c: 0){ _LL180: _temp179=(( struct Cyc_Absyn_String_c_struct*)
_temp153)->f1; goto _LL166;} else{ goto _LL167;} _LL167: if( _temp153 ==( void*)
Cyc_Absyn_Null_c){ goto _LL168;} else{ goto _LL154;} _LL156: t= _temp169 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL154;
_LL158: t= _temp171 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL154; _LL160: t= _temp173 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL154; _LL162: t= Cyc_Absyn_float_t; goto _LL154;
_LL164: if( topt == 0){ t= _temp177 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp181= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp191; void* _temp193; void* _temp195; void*
_temp197; void* _temp199; _LL183: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; if( _temp191 ==( void*) Cyc_Absyn_B1){ goto _LL184;} else{ goto
_LL185;}} else{ goto _LL185;} _LL185: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; if( _temp195 ==( void*) Cyc_Absyn_B2){ goto _LL186;} else{ goto
_LL187;}} else{ goto _LL187;} _LL187: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL188;} else{ goto _LL189;} _LL189: goto _LL190; _LL184: t=
_temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp201=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp201[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp202; _temp202.tag= Cyc_Absyn_Const_e;
_temp202.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp203=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp203[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp204; _temp204.tag= Cyc_Absyn_Char_c;
_temp204.f1=( void*) _temp193; _temp204.f2=( unsigned char) _temp175; _temp204;});
_temp203;})); _temp202;}); _temp201;}))); goto _LL182; _LL186: t= _temp197 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp205=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp206; _temp206.tag= Cyc_Absyn_Const_e; _temp206.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp207=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp207[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp208; _temp208.tag= Cyc_Absyn_Short_c; _temp208.f1=( void*) _temp197;
_temp208.f2=( short) _temp175; _temp208;}); _temp207;})); _temp206;}); _temp205;})));
goto _LL182; _LL188: t= _temp199 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL182; _LL190: t= _temp177 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL182; _LL182:;} goto _LL154; _LL166: {
int len=( int) _get_arr_size( _temp179, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp209=({ struct Cyc_Absyn_Const_e_struct* _temp246=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp246[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp247; _temp247.tag= Cyc_Absyn_Const_e; _temp247.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp248=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp248[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp249; _temp249.tag= Cyc_Absyn_Int_c; _temp249.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp249.f2= len; _temp249;}); _temp248;})); _temp247;}); _temp246;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp209, loc); elen->topt=({
struct Cyc_Core_Opt* _temp210=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp210->v=( void*) Cyc_Absyn_uint_t; _temp210;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp211=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp211[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp212; _temp212.tag= Cyc_Absyn_Upper_b; _temp212.f1= elen; _temp212;});
_temp211;})); if( topt != 0){ void* _temp213= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp221; struct Cyc_Absyn_Tqual
_temp223; void* _temp225; struct Cyc_Absyn_PtrInfo _temp227; struct Cyc_Absyn_Conref*
_temp229; struct Cyc_Absyn_Tqual _temp231; struct Cyc_Absyn_Conref* _temp233;
void* _temp235; void* _temp237; _LL215: if(( unsigned int) _temp213 > 4u?*(( int*)
_temp213) == Cyc_Absyn_ArrayType: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f1; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f2; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f3; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp213 > 4u?*(( int*) _temp213) == Cyc_Absyn_PointerType: 0){ _LL228: _temp227=((
struct Cyc_Absyn_PointerType_struct*) _temp213)->f1; _LL238: _temp237=( void*)
_temp227.elt_typ; goto _LL236; _LL236: _temp235=( void*) _temp227.rgn_typ; goto
_LL234; _LL234: _temp233= _temp227.nullable; goto _LL232; _LL232: _temp231=
_temp227.tq; goto _LL230; _LL230: _temp229= _temp227.bounds; goto _LL218;} else{
goto _LL219;} _LL219: goto _LL220; _LL216: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp239=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp239[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp240; _temp240.tag= Cyc_Absyn_ArrayType;
_temp240.f1=( void*) Cyc_Absyn_uchar_t; _temp240.f2= _temp223; _temp240.f3=(
struct Cyc_Absyn_Exp*) elen; _temp240;}); _temp239;}); _LL218: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
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
goto _LL214; _LL214:;} return t;} _LL168: { struct Cyc_List_List* _temp250= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp251=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp251[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp252; _temp252.tag= Cyc_Absyn_PointerType;
_temp252.f1=({ struct Cyc_Absyn_PtrInfo _temp253; _temp253.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp255=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*)
_temp250; _temp255;})); _temp253.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp254=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp254->v=( void*) _temp250;
_temp254;})); _temp253.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp253.tq= Cyc_Absyn_empty_tqual(); _temp253.bounds= Cyc_Absyn_empty_conref();
_temp253;}); _temp252;}); _temp251;}); goto _LL154;} _LL154:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp256= b; struct Cyc_Absyn_Vardecl*
_temp270; struct Cyc_Absyn_Fndecl* _temp272; struct Cyc_Absyn_Vardecl* _temp274;
struct Cyc_Absyn_Vardecl* _temp276; struct Cyc_Absyn_Vardecl* _temp278; _LL258:
if( _temp256 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL259;} else{ goto _LL260;}
_LL260: if(( unsigned int) _temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Global_b:
0){ _LL271: _temp270=(( struct Cyc_Absyn_Global_b_struct*) _temp256)->f1; goto
_LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp256 > 1u?*(( int*)
_temp256) == Cyc_Absyn_Funname_b: 0){ _LL273: _temp272=(( struct Cyc_Absyn_Funname_b_struct*)
_temp256)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Pat_b: 0){ _LL275: _temp274=((
struct Cyc_Absyn_Pat_b_struct*) _temp256)->f1; goto _LL265;} else{ goto _LL266;}
_LL266: if(( unsigned int) _temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Local_b:
0){ _LL277: _temp276=(( struct Cyc_Absyn_Local_b_struct*) _temp256)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: if(( unsigned int) _temp256 > 1u?*(( int*)
_temp256) == Cyc_Absyn_Param_b: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Param_b_struct*)
_temp256)->f1; goto _LL269;} else{ goto _LL257;} _LL259: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp280)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp284= _tag_arr("unbound identifier %s", sizeof( unsigned char), 22u); struct
Cyc_Stdio_String_pa_struct _temp285; _temp285.tag= Cyc_Stdio_String_pa; _temp285.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp283=( void*)&
_temp285; void* _temp281[ 1u]={ _temp283}; struct _tagged_arr _temp282={( void*)
_temp281,( void*) _temp281,( void*)( _temp281 + 1u)}; _temp280( _temp284,
_temp282);}})); _LL261: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp270->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp270->type; _LL263:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp272->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp272);
_LL265: _temp276= _temp274; goto _LL267; _LL267: _temp278= _temp276; goto _LL269;
_LL269:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp278->type; _LL257:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp286=( void*) fmt->r; void* _temp292; struct _tagged_arr
_temp294; _LL288: if(*(( int*) _temp286) == Cyc_Absyn_Const_e){ _LL293: _temp292=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp286)->f1; if(( unsigned int)
_temp292 > 1u?*(( int*) _temp292) == Cyc_Absyn_String_c: 0){ _LL295: _temp294=((
struct Cyc_Absyn_String_c_struct*) _temp292)->f1; goto _LL289;} else{ goto
_LL290;}} else{ goto _LL290;} _LL290: goto _LL291; _LL289: desc_types=
type_getter( te, _temp294, fmt->loc); goto _LL287; _LL291: Cyc_Tcutil_terr( fmt->loc,
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));
return; _LL287:;} for( 0; desc_types != 0? args != 0: 0;( desc_types=(( struct
Cyc_List_List*) _check_null( desc_types))->tl, args=(( struct Cyc_List_List*)
_check_null( args))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp296)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp301= _tag_arr("descriptor has type %s but argument has type %s", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp303; _temp303.tag=
Cyc_Stdio_String_pa; _temp303.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp299=( void*)& _temp303; struct Cyc_Stdio_String_pa_struct _temp302;
_temp302.tag= Cyc_Stdio_String_pa; _temp302.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp300=(
void*)& _temp302; void* _temp297[ 2u]={ _temp299, _temp300}; struct _tagged_arr
_temp298={( void*) _temp297,( void*) _temp297,( void*)( _temp297 + 2u)};
_temp296( _temp301, _temp298);}}}));}} if( desc_types != 0){ Cyc_Tcutil_terr(
fmt->loc, _tag_arr("too few arguments", sizeof( unsigned char), 18u));} if( args
!= 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->loc, _tag_arr("too many arguments", sizeof(
unsigned char), 19u));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp304= p; _LL306: if( _temp304 ==( void*) Cyc_Absyn_Plus){ goto _LL307;}
else{ goto _LL308;} _LL308: if( _temp304 ==( void*) Cyc_Absyn_Minus){ goto
_LL309;} else{ goto _LL310;} _LL310: if( _temp304 ==( void*) Cyc_Absyn_Not){
goto _LL311;} else{ goto _LL312;} _LL312: if( _temp304 ==( void*) Cyc_Absyn_Bitnot){
goto _LL313;} else{ goto _LL314;} _LL314: if( _temp304 ==( void*) Cyc_Absyn_Size){
goto _LL315;} else{ goto _LL316;} _LL316: goto _LL317; _LL307: goto _LL309;
_LL309: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp318)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp322= _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp323;
_temp323.tag= Cyc_Stdio_String_pa; _temp323.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp321=( void*)& _temp323; void* _temp319[ 1u]={ _temp321}; struct
_tagged_arr _temp320={( void*) _temp319,( void*) _temp319,( void*)( _temp319 + 1u)};
_temp318( _temp322, _temp320);}}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL311: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp324)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp328= _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp329;
_temp329.tag= Cyc_Stdio_String_pa; _temp329.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp327=( void*)& _temp329; void* _temp325[ 1u]={ _temp327}; struct
_tagged_arr _temp326={( void*) _temp325,( void*) _temp325,( void*)( _temp325 + 1u)};
_temp324( _temp328, _temp326);}}));} return Cyc_Absyn_sint_t; _LL313: if( ! Cyc_Tcutil_is_integral(
e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp330)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp334= _tag_arr("expecting integral type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp335; _temp335.tag=
Cyc_Stdio_String_pa; _temp335.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp333=( void*)& _temp335; void* _temp331[ 1u]={ _temp333}; struct
_tagged_arr _temp332={( void*) _temp331,( void*) _temp331,( void*)( _temp331 + 1u)};
_temp330( _temp334, _temp332);}}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL315:{ void* _temp336= t; struct Cyc_Absyn_PtrInfo
_temp344; _LL338: if(( unsigned int) _temp336 > 4u?*(( int*) _temp336) == Cyc_Absyn_ArrayType:
0){ goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp336 > 4u?*((
int*) _temp336) == Cyc_Absyn_PointerType: 0){ _LL345: _temp344=(( struct Cyc_Absyn_PointerType_struct*)
_temp336)->f1; goto _LL341;} else{ goto _LL342;} _LL342: goto _LL343; _LL339:
goto _LL337; _LL341: goto _LL337; _LL343: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp346)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp350= _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp351;
_temp351.tag= Cyc_Stdio_String_pa; _temp351.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp349=( void*)& _temp351; void* _temp347[ 1u]={ _temp349}; struct
_tagged_arr _temp348={( void*) _temp347,( void*) _temp347,( void*)( _temp347 + 1u)};
_temp346( _temp350, _temp348);}})); goto _LL337; _LL337:;} return Cyc_Absyn_uint_t;
_LL317: return Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof(
unsigned char), 17u)); _LL305:;} static void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct
Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp352)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp356= _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp357;
_temp357.tag= Cyc_Stdio_String_pa; _temp357.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp355=(
void*)& _temp357; void* _temp353[ 1u]={ _temp355}; struct _tagged_arr _temp354={(
void*) _temp353,( void*) _temp353,( void*)( _temp353 + 1u)}; _temp352( _temp356,
_temp354);}})); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp358=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp358->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp358;}));} if( ! checker( e2)){ Cyc_Tcutil_terr(
e2->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp359)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp363= _tag_arr("type %s cannot be used here", sizeof( unsigned char), 28u);
struct Cyc_Stdio_String_pa_struct _temp364; _temp364.tag= Cyc_Stdio_String_pa;
_temp364.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ void* _temp362=( void*)& _temp364; void* _temp360[
1u]={ _temp362}; struct _tagged_arr _temp361={( void*) _temp360,( void*)
_temp360,( void*)( _temp360 + 1u)}; _temp359( _temp363, _temp361);}})); return
Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp365=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp365->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp365;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp366= t1; struct
Cyc_Absyn_PtrInfo _temp372; struct Cyc_Absyn_Conref* _temp374; struct Cyc_Absyn_Tqual
_temp376; struct Cyc_Absyn_Conref* _temp378; void* _temp380; void* _temp382;
_LL368: if(( unsigned int) _temp366 > 4u?*(( int*) _temp366) == Cyc_Absyn_PointerType:
0){ _LL373: _temp372=(( struct Cyc_Absyn_PointerType_struct*) _temp366)->f1;
_LL383: _temp382=( void*) _temp372.elt_typ; goto _LL381; _LL381: _temp380=( void*)
_temp372.rgn_typ; goto _LL379; _LL379: _temp378= _temp372.nullable; goto _LL377;
_LL377: _temp376= _temp372.tq; goto _LL375; _LL375: _temp374= _temp372.bounds;
goto _LL369;} else{ goto _LL370;} _LL370: goto _LL371; _LL369: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp384)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp388= _tag_arr("expecting int but found %s", sizeof(
unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp389; _temp389.tag=
Cyc_Stdio_String_pa; _temp389.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp387=( void*)& _temp389; void* _temp385[ 1u]={ _temp387}; struct
_tagged_arr _temp386={( void*) _temp385,( void*) _temp385,( void*)( _temp385 + 1u)};
_temp384( _temp388, _temp386);}}));} _temp374= Cyc_Absyn_compress_conref(
_temp374);{ void* _temp390=( void*) _temp374->v; void* _temp398; void* _temp400;
struct Cyc_Absyn_Exp* _temp402; _LL392: if(( unsigned int) _temp390 > 1u?*(( int*)
_temp390) == Cyc_Absyn_Eq_constr: 0){ _LL399: _temp398=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp390)->f1; if( _temp398 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL393;} else{
goto _LL394;}} else{ goto _LL394;} _LL394: if(( unsigned int) _temp390 > 1u?*((
int*) _temp390) == Cyc_Absyn_Eq_constr: 0){ _LL401: _temp400=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp390)->f1; if(( unsigned int) _temp400 > 1u?*((
int*) _temp400) == Cyc_Absyn_Upper_b: 0){ _LL403: _temp402=(( struct Cyc_Absyn_Upper_b_struct*)
_temp400)->f1; goto _LL395;} else{ goto _LL396;}} else{ goto _LL396;} _LL396:
goto _LL397; _LL393: return t1; _LL395: { struct Cyc_Absyn_PointerType_struct*
_temp404=({ struct Cyc_Absyn_PointerType_struct* _temp405=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp405[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp406; _temp406.tag= Cyc_Absyn_PointerType;
_temp406.f1=({ struct Cyc_Absyn_PtrInfo _temp407; _temp407.elt_typ=( void*)
_temp382; _temp407.rgn_typ=( void*) _temp380; _temp407.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp407.tq= _temp376; _temp407.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp407;}); _temp406;}); _temp405;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp404); return( void*) _temp404;} _LL397:( void*)( _temp374->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp408=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp408[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp409; _temp409.tag= Cyc_Absyn_Eq_constr; _temp409.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp409;}); _temp408;}))); return t1;
_LL391:;} _LL371: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL367:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp410)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp415= _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp417;
_temp417.tag= Cyc_Stdio_String_pa; _temp417.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp413=(
void*)& _temp417; struct Cyc_Stdio_String_pa_struct _temp416; _temp416.tag= Cyc_Stdio_String_pa;
_temp416.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ void* _temp414=( void*)& _temp416; void* _temp411[
2u]={ _temp413, _temp414}; struct _tagged_arr _temp412={( void*) _temp411,( void*)
_temp411,( void*)( _temp411 + 2u)}; _temp410( _temp415, _temp412);}}}));} return
Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr(
e2->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp418)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp423= _tag_arr("expecting either %s or int but found %s", sizeof(
unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp425; _temp425.tag=
Cyc_Stdio_String_pa; _temp425.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp421=( void*)& _temp425; struct Cyc_Stdio_String_pa_struct
_temp424; _temp424.tag= Cyc_Stdio_String_pa; _temp424.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp422=( void*)& _temp424; void* _temp419[
2u]={ _temp421, _temp422}; struct _tagged_arr _temp420={( void*) _temp419,( void*)
_temp419,( void*)( _temp419 + 2u)}; _temp418( _temp423, _temp420);}}}));} return
t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp426=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp426->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp426;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp428=({ struct _tuple0 _temp427;
_temp427.f1= Cyc_Tcutil_compress( t1); _temp427.f2= Cyc_Tcutil_compress( t2);
_temp427;}); void* _temp434; struct Cyc_Absyn_PtrInfo _temp436; void* _temp438;
void* _temp440; struct Cyc_Absyn_PtrInfo _temp442; void* _temp444; _LL430:
_LL441: _temp440= _temp428.f1; if(( unsigned int) _temp440 > 4u?*(( int*)
_temp440) == Cyc_Absyn_PointerType: 0){ _LL443: _temp442=(( struct Cyc_Absyn_PointerType_struct*)
_temp440)->f1; _LL445: _temp444=( void*) _temp442.elt_typ; goto _LL435;} else{
goto _LL432;} _LL435: _temp434= _temp428.f2; if(( unsigned int) _temp434 > 4u?*((
int*) _temp434) == Cyc_Absyn_PointerType: 0){ _LL437: _temp436=(( struct Cyc_Absyn_PointerType_struct*)
_temp434)->f1; _LL439: _temp438=( void*) _temp436.elt_typ; goto _LL431;} else{
goto _LL432;} _LL432: goto _LL433; _LL431: if( Cyc_Tcutil_unify( _temp444,
_temp438)){ return Cyc_Absyn_sint_t;} goto _LL429; _LL433: goto _LL429; _LL429:;}
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp446)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp451= _tag_arr("comparison not allowed between %s and %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp453;
_temp453.tag= Cyc_Stdio_String_pa; _temp453.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp449=( void*)& _temp453; struct Cyc_Stdio_String_pa_struct
_temp452; _temp452.tag= Cyc_Stdio_String_pa; _temp452.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp450=( void*)& _temp452; void* _temp447[
2u]={ _temp449, _temp450}; struct _tagged_arr _temp448={( void*) _temp447,( void*)
_temp447,( void*)( _temp447 + 2u)}; _temp446( _temp451, _temp448);}}})); return
Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp454=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp454->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp454;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp455= p; _LL457: if( _temp455 ==( void*)
Cyc_Absyn_Plus){ goto _LL458;} else{ goto _LL459;} _LL459: if( _temp455 ==( void*)
Cyc_Absyn_Minus){ goto _LL460;} else{ goto _LL461;} _LL461: if( _temp455 ==(
void*) Cyc_Absyn_Times){ goto _LL462;} else{ goto _LL463;} _LL463: if( _temp455
==( void*) Cyc_Absyn_Div){ goto _LL464;} else{ goto _LL465;} _LL465: if(
_temp455 ==( void*) Cyc_Absyn_Mod){ goto _LL466;} else{ goto _LL467;} _LL467:
if( _temp455 ==( void*) Cyc_Absyn_Bitand){ goto _LL468;} else{ goto _LL469;}
_LL469: if( _temp455 ==( void*) Cyc_Absyn_Bitor){ goto _LL470;} else{ goto
_LL471;} _LL471: if( _temp455 ==( void*) Cyc_Absyn_Bitxor){ goto _LL472;} else{
goto _LL473;} _LL473: if( _temp455 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL474;}
else{ goto _LL475;} _LL475: if( _temp455 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL476;} else{ goto _LL477;} _LL477: if( _temp455 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL478;} else{ goto _LL479;} _LL479: if( _temp455 ==( void*) Cyc_Absyn_Eq){
goto _LL480;} else{ goto _LL481;} _LL481: if( _temp455 ==( void*) Cyc_Absyn_Neq){
goto _LL482;} else{ goto _LL483;} _LL483: if( _temp455 ==( void*) Cyc_Absyn_Gt){
goto _LL484;} else{ goto _LL485;} _LL485: if( _temp455 ==( void*) Cyc_Absyn_Lt){
goto _LL486;} else{ goto _LL487;} _LL487: if( _temp455 ==( void*) Cyc_Absyn_Gte){
goto _LL488;} else{ goto _LL489;} _LL489: if( _temp455 ==( void*) Cyc_Absyn_Lte){
goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL458: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL460: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL462: goto _LL464;
_LL464: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL466: goto _LL468; _LL468: goto _LL470; _LL470: goto _LL472; _LL472: goto
_LL474; _LL474: goto _LL476; _LL476: goto _LL478; _LL478: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL480: goto _LL482; _LL482: goto _LL484;
_LL484: goto _LL486; _LL486: goto _LL488; _LL488: goto _LL490; _LL490: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL492:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp493=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp493[ 0]=({ struct Cyc_Core_Failure_struct
_temp494; _temp494.tag= Cyc_Core_Failure; _temp494.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp494;}); _temp493;})); _LL456:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp495= p; _LL497: if( _temp495
==( void*) Cyc_Absyn_Printf){ goto _LL498;} else{ goto _LL499;} _LL499: if(
_temp495 ==( void*) Cyc_Absyn_Fprintf){ goto _LL500;} else{ goto _LL501;} _LL501:
if( _temp495 ==( void*) Cyc_Absyn_Aprintf){ goto _LL502;} else{ goto _LL503;}
_LL503: if( _temp495 ==( void*) Cyc_Absyn_Scanf){ goto _LL504;} else{ goto
_LL505;} _LL505: if( _temp495 ==( void*) Cyc_Absyn_Fscanf){ goto _LL506;} else{
goto _LL507;} _LL507: if( _temp495 ==( void*) Cyc_Absyn_Sscanf){ goto _LL508;}
else{ goto _LL509;} _LL509: goto _LL510; _LL498: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("missing format string in printf", sizeof( unsigned char), 32u));}
Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd,(( struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL496; _LL500: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fprintf",
sizeof( unsigned char), 29u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp511)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp515= _tag_arr("expecting FILE @ but found %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp516; _temp516.tag=
Cyc_Stdio_String_pa; _temp516.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v);{ void* _temp514=( void*)&
_temp516; void* _temp512[ 1u]={ _temp514}; struct _tagged_arr _temp513={( void*)
_temp512,( void*) _temp512,( void*)( _temp512 + 1u)}; _temp511( _temp515,
_temp513);}}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL496;
_LL502: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in aprintf",
sizeof( unsigned char), 33u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt*
_temp517=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp517->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp517;}))); goto _LL496;
_LL504: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in scanf",
sizeof( unsigned char), 31u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL496; _LL506: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fscanf",
sizeof( unsigned char), 28u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp518)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp522= _tag_arr("expecting FILE @ but found %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp523; _temp523.tag=
Cyc_Stdio_String_pa; _temp523.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v);{ void* _temp521=( void*)&
_temp523; void* _temp519[ 1u]={ _temp521}; struct _tagged_arr _temp520={( void*)
_temp519,( void*) _temp519,( void*)( _temp519 + 1u)}; _temp518( _temp522,
_temp520);}}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL496; _LL508: Cyc_Tcutil_terr(
loc, _tag_arr("sscanf is not supported", sizeof( unsigned char), 24u)); t= Cyc_Absyn_sint_t;
goto _LL496; _LL510: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ case 0: _LL524: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL525: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL526: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL527: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL496; _LL496:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp529=( void*) e->r;
void* _temp553; struct Cyc_Absyn_Vardecl* _temp555; void* _temp557; struct Cyc_Absyn_Vardecl*
_temp559; void* _temp561; struct Cyc_Absyn_Vardecl* _temp563; void* _temp565;
struct Cyc_Absyn_Vardecl* _temp567; struct Cyc_Absyn_Exp* _temp569; struct Cyc_Absyn_Exp*
_temp571; struct _tagged_arr* _temp573; struct Cyc_Absyn_Exp* _temp575; struct
_tagged_arr* _temp577; struct Cyc_Absyn_Exp* _temp579; struct Cyc_Absyn_Exp*
_temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Exp* _temp585; _LL531:
if(*(( int*) _temp529) == Cyc_Absyn_Var_e){ _LL554: _temp553=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp529)->f2; if(( unsigned int) _temp553 > 1u?*(( int*)
_temp553) == Cyc_Absyn_Param_b: 0){ _LL556: _temp555=(( struct Cyc_Absyn_Param_b_struct*)
_temp553)->f1; goto _LL532;} else{ goto _LL533;}} else{ goto _LL533;} _LL533:
if(*(( int*) _temp529) == Cyc_Absyn_Var_e){ _LL558: _temp557=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp529)->f2; if(( unsigned int) _temp557 > 1u?*(( int*)
_temp557) == Cyc_Absyn_Local_b: 0){ _LL560: _temp559=(( struct Cyc_Absyn_Local_b_struct*)
_temp557)->f1; goto _LL534;} else{ goto _LL535;}} else{ goto _LL535;} _LL535:
if(*(( int*) _temp529) == Cyc_Absyn_Var_e){ _LL562: _temp561=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp529)->f2; if(( unsigned int) _temp561 > 1u?*(( int*)
_temp561) == Cyc_Absyn_Pat_b: 0){ _LL564: _temp563=(( struct Cyc_Absyn_Pat_b_struct*)
_temp561)->f1; goto _LL536;} else{ goto _LL537;}} else{ goto _LL537;} _LL537:
if(*(( int*) _temp529) == Cyc_Absyn_Var_e){ _LL566: _temp565=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp529)->f2; if(( unsigned int) _temp565 > 1u?*(( int*)
_temp565) == Cyc_Absyn_Global_b: 0){ _LL568: _temp567=(( struct Cyc_Absyn_Global_b_struct*)
_temp565)->f1; goto _LL538;} else{ goto _LL539;}} else{ goto _LL539;} _LL539:
if(*(( int*) _temp529) == Cyc_Absyn_Subscript_e){ _LL572: _temp571=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp529)->f1; goto _LL570; _LL570: _temp569=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp529)->f2; goto _LL540;} else{ goto _LL541;} _LL541: if(*(( int*) _temp529)
== Cyc_Absyn_StructMember_e){ _LL576: _temp575=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp529)->f1; goto _LL574; _LL574: _temp573=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp529)->f2; goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp529)
== Cyc_Absyn_StructArrow_e){ _LL580: _temp579=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp529)->f1; goto _LL578; _LL578: _temp577=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp529)->f2; goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp529)
== Cyc_Absyn_Deref_e){ _LL582: _temp581=(( struct Cyc_Absyn_Deref_e_struct*)
_temp529)->f1; goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp529)
== Cyc_Absyn_NoInstantiate_e){ _LL584: _temp583=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp529)->f1; goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp529)
== Cyc_Absyn_Instantiate_e){ _LL586: _temp585=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp529)->f1; goto _LL550;} else{ goto _LL551;} _LL551: goto _LL552; _LL532:
_temp559= _temp555; goto _LL534; _LL534: _temp563= _temp559; goto _LL536; _LL536:
_temp567= _temp563; goto _LL538; _LL538: if( !( _temp567->tq).q_const){ return;}
goto _LL530; _LL540:{ void* _temp587= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp571->topt))->v); struct Cyc_Absyn_PtrInfo _temp597; struct Cyc_Absyn_Tqual
_temp599; struct Cyc_Absyn_Tqual _temp601; struct Cyc_List_List* _temp603;
_LL589: if(( unsigned int) _temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_PointerType:
0){ _LL598: _temp597=(( struct Cyc_Absyn_PointerType_struct*) _temp587)->f1;
_LL600: _temp599= _temp597.tq; goto _LL590;} else{ goto _LL591;} _LL591: if((
unsigned int) _temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_ArrayType: 0){
_LL602: _temp601=(( struct Cyc_Absyn_ArrayType_struct*) _temp587)->f2; goto
_LL592;} else{ goto _LL593;} _LL593: if(( unsigned int) _temp587 > 4u?*(( int*)
_temp587) == Cyc_Absyn_TupleType: 0){ _LL604: _temp603=(( struct Cyc_Absyn_TupleType_struct*)
_temp587)->f1; goto _LL594;} else{ goto _LL595;} _LL595: goto _LL596; _LL590:
_temp601= _temp599; goto _LL592; _LL592: if( ! _temp601.q_const){ return;} goto
_LL588; _LL594: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp569);{
struct _handler_cons _temp605; _push_handler(& _temp605);{ int _temp607= 0; if(
setjmp( _temp605.handler)){ _temp607= 1;} if( ! _temp607){{ struct _tuple8
_temp610; struct Cyc_Absyn_Tqual _temp611; struct _tuple8* _temp608=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp603,( int) i);
_temp610=* _temp608; _LL612: _temp611= _temp610.f1; goto _LL609; _LL609: if( !
_temp611.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp606=( void*) _exn_thrown; void* _temp614= _temp606; _LL616: if( _temp614 ==
Cyc_List_Nth){ goto _LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617:
return; _LL619:( void) _throw( _temp614); _LL615:;}}} goto _LL588;} _LL596: goto
_LL588; _LL588:;} goto _LL530; _LL542:{ void* _temp620= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp575->topt))->v); struct Cyc_Absyn_Structdecl**
_temp632; struct Cyc_List_List* _temp634; struct Cyc_List_List* _temp636; struct
Cyc_Absyn_Uniondecl** _temp638; _LL622: if(( unsigned int) _temp620 > 4u?*(( int*)
_temp620) == Cyc_Absyn_StructType: 0){ _LL633: _temp632=(( struct Cyc_Absyn_StructType_struct*)
_temp620)->f3; goto _LL623;} else{ goto _LL624;} _LL624: if(( unsigned int)
_temp620 > 4u?*(( int*) _temp620) == Cyc_Absyn_AnonUnionType: 0){ _LL635:
_temp634=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp620)->f1; goto _LL625;}
else{ goto _LL626;} _LL626: if(( unsigned int) _temp620 > 4u?*(( int*) _temp620)
== Cyc_Absyn_AnonStructType: 0){ _LL637: _temp636=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp620)->f1; goto _LL627;} else{ goto _LL628;} _LL628: if(( unsigned int)
_temp620 > 4u?*(( int*) _temp620) == Cyc_Absyn_UnionType: 0){ _LL639: _temp638=((
struct Cyc_Absyn_UnionType_struct*) _temp620)->f3; goto _LL629;} else{ goto
_LL630;} _LL630: goto _LL631; _LL623: { struct Cyc_Absyn_Structfield* sf=
_temp632 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp632)), _temp573); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL621;} _LL625: _temp636= _temp634; goto _LL627; _LL627: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp636, _temp573); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL621;} _LL629: { struct Cyc_Absyn_Structfield*
sf= _temp638 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp638)), _temp573); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL621;} _LL631: goto _LL621; _LL621:;} goto _LL530; _LL544:{ void*
_temp640= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp579->topt))->v); struct Cyc_Absyn_PtrInfo _temp646; void* _temp648; _LL642:
if(( unsigned int) _temp640 > 4u?*(( int*) _temp640) == Cyc_Absyn_PointerType: 0){
_LL647: _temp646=(( struct Cyc_Absyn_PointerType_struct*) _temp640)->f1; _LL649:
_temp648=( void*) _temp646.elt_typ; goto _LL643;} else{ goto _LL644;} _LL644:
goto _LL645; _LL643:{ void* _temp650= Cyc_Tcutil_compress( _temp648); struct Cyc_Absyn_Structdecl**
_temp662; struct Cyc_Absyn_Uniondecl** _temp664; struct Cyc_List_List* _temp666;
struct Cyc_List_List* _temp668; _LL652: if(( unsigned int) _temp650 > 4u?*(( int*)
_temp650) == Cyc_Absyn_StructType: 0){ _LL663: _temp662=(( struct Cyc_Absyn_StructType_struct*)
_temp650)->f3; goto _LL653;} else{ goto _LL654;} _LL654: if(( unsigned int)
_temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_UnionType: 0){ _LL665: _temp664=((
struct Cyc_Absyn_UnionType_struct*) _temp650)->f3; goto _LL655;} else{ goto
_LL656;} _LL656: if(( unsigned int) _temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_AnonStructType:
0){ _LL667: _temp666=(( struct Cyc_Absyn_AnonStructType_struct*) _temp650)->f1;
goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int) _temp650 > 4u?*((
int*) _temp650) == Cyc_Absyn_AnonUnionType: 0){ _LL669: _temp668=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp650)->f1; goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL653: {
struct Cyc_Absyn_Structfield* sf= _temp662 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp662)), _temp577); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL651;} _LL655: { struct Cyc_Absyn_Structfield*
sf= _temp664 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp664)), _temp577); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL651;} _LL657: _temp668= _temp666; goto _LL659; _LL659: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp668, _temp577); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL651;} _LL661: goto _LL651;
_LL651:;} goto _LL641; _LL645: goto _LL641; _LL641:;} goto _LL530; _LL546:{ void*
_temp670= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp581->topt))->v); struct Cyc_Absyn_PtrInfo _temp678; struct Cyc_Absyn_Tqual
_temp680; struct Cyc_Absyn_Tqual _temp682; _LL672: if(( unsigned int) _temp670 >
4u?*(( int*) _temp670) == Cyc_Absyn_PointerType: 0){ _LL679: _temp678=(( struct
Cyc_Absyn_PointerType_struct*) _temp670)->f1; _LL681: _temp680= _temp678.tq;
goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int) _temp670 > 4u?*((
int*) _temp670) == Cyc_Absyn_ArrayType: 0){ _LL683: _temp682=(( struct Cyc_Absyn_ArrayType_struct*)
_temp670)->f2; goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL673:
_temp682= _temp680; goto _LL675; _LL675: if( ! _temp682.q_const){ return;} goto
_LL671; _LL677: goto _LL671; _LL671:;} goto _LL530; _LL548: _temp585= _temp583;
goto _LL550; _LL550: Cyc_Tcexp_check_writable( te, _temp585); return; _LL552:
goto _LL530; _LL530:;} Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp684)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp688= _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp689;
_temp689.tag= Cyc_Stdio_String_pa; _temp689.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp687=( void*)& _temp689; void* _temp685[ 1u]={ _temp687}; struct
_tagged_arr _temp686={( void*) _temp685,( void*) _temp685,( void*)( _temp685 + 1u)};
_temp684( _temp688, _temp686);}}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp702=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp702->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp702;})); struct Cyc_List_List* _temp690=({
struct Cyc_List_List* _temp701=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp701->hd=( void*) e3; _temp701->tl= 0; _temp701;});
struct Cyc_List_List* _temp691=({ struct Cyc_List_List* _temp700=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp700->hd=( void*) e2; _temp700->tl=
_temp690; _temp700;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp691)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp692)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp697=
_tag_arr("conditional clause types do not match: %s != %s", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp699; _temp699.tag=
Cyc_Stdio_String_pa; _temp699.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp695=(
void*)& _temp699; struct Cyc_Stdio_String_pa_struct _temp698; _temp698.tag= Cyc_Stdio_String_pa;
_temp698.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ void* _temp696=( void*)& _temp698; void* _temp693[
2u]={ _temp695, _temp696}; struct _tagged_arr _temp694={( void*) _temp693,( void*)
_temp693,( void*)( _temp693 + 2u)}; _temp692( _temp697, _temp694);}}}));} return
t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)((
void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)), e2);{ void* t1=(
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp703= Cyc_Tcutil_compress(
t1); _LL705: if(( unsigned int) _temp703 > 4u?*(( int*) _temp703) == Cyc_Absyn_ArrayType:
0){ goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL704; _LL708: goto _LL704; _LL704:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp709)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp714= _tag_arr("type mismatch: %s != %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp716;
_temp716.tag= Cyc_Stdio_String_pa; _temp716.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp712=( void*)& _temp716; struct Cyc_Stdio_String_pa_struct
_temp715; _temp715.tag= Cyc_Stdio_String_pa; _temp715.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp713=( void*)& _temp715; void* _temp710[
2u]={ _temp712, _temp713}; struct _tagged_arr _temp711={( void*) _temp710,( void*)
_temp710,( void*)( _temp710 + 2u)}; _temp709( _temp714, _temp711);}}}));}} else{
void* _temp717=( void*)(( struct Cyc_Core_Opt*) _check_null( po))->v; void*
_temp718= Cyc_Tcexp_tcBinPrimop( te, loc, 0, _temp717, e1, e2); if( !( Cyc_Tcutil_unify(
_temp718, t1)? 1: Cyc_Tcutil_coerceable( _temp718))){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("Cannot use this operator in an assignment", sizeof(
unsigned char), 42u));} return _temp718;} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp719= Cyc_Tcutil_compress(
t1); void* _temp729; void* _temp731; _LL721: if( _temp719 ==( void*) Cyc_Absyn_FloatType){
goto _LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp719 > 4u?*((
int*) _temp719) == Cyc_Absyn_IntType: 0){ _LL730: _temp729=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp719)->f2; if( _temp729 ==( void*) Cyc_Absyn_B1){ goto _LL724;} else{ goto
_LL725;}} else{ goto _LL725;} _LL725: if(( unsigned int) _temp719 > 4u?*(( int*)
_temp719) == Cyc_Absyn_IntType: 0){ _LL732: _temp731=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp719)->f2; if( _temp731 ==( void*) Cyc_Absyn_B2){ goto _LL726;} else{ goto
_LL727;}} else{ goto _LL727;} _LL727: goto _LL728; _LL722: Cyc_Tcutil_unchecked_cast(
te, e,( void*) Cyc_Absyn_DoubleType); t1=( void*) Cyc_Absyn_DoubleType; goto
_LL720; _LL724: goto _LL726; _LL726: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL720; _LL728: goto _LL720; _LL720:;} for( fields=
fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
void* _temp735; struct Cyc_Position_Segment* _temp737; struct Cyc_List_List*
_temp739; struct Cyc_List_List* _temp741; struct _tuple1* _temp743; struct Cyc_Absyn_Tunionfield
_temp733=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL744: _temp743= _temp733.name; goto _LL742; _LL742: _temp741=
_temp733.tvs; goto _LL740; _LL740: _temp739= _temp733.typs; goto _LL738; _LL738:
_temp737= _temp733.loc; goto _LL736; _LL736: _temp735=( void*) _temp733.sc; goto
_LL734; _LL734: if(( _temp739 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp739))->tl != 0)? 1: _temp741 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp739))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp747; struct Cyc_Position_Segment* _temp749; struct Cyc_List_List*
_temp751; struct Cyc_List_List* _temp753; struct _tuple1* _temp755; struct Cyc_Absyn_Tunionfield
_temp745=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL756: _temp755= _temp745.name; goto _LL754; _LL754: _temp753=
_temp745.tvs; goto _LL752; _LL752: _temp751= _temp745.typs; goto _LL750; _LL750:
_temp749= _temp745.loc; goto _LL748; _LL748: _temp747=( void*) _temp745.sc; goto
_LL746; _LL746: if(( _temp751 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp751))->tl != 0)? 1: _temp753 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp751))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp757)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp762= _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp764;
_temp764.tag= Cyc_Stdio_String_pa; _temp764.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp760=( void*)& _temp764; struct Cyc_Stdio_String_pa_struct
_temp763; _temp763.tag= Cyc_Stdio_String_pa; _temp763.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp761=( void*)& _temp763; void* _temp758[
2u]={ _temp760, _temp761}; struct _tagged_arr _temp759={( void*) _temp758,( void*)
_temp758,( void*)( _temp758 + 2u)}; _temp757( _temp762, _temp759);}}})); return
0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){ te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp765= t; struct Cyc_Absyn_PtrInfo _temp771; struct Cyc_Absyn_Conref*
_temp773; struct Cyc_Absyn_Tqual _temp775; struct Cyc_Absyn_Conref* _temp777;
void* _temp779; void* _temp781; _LL767: if(( unsigned int) _temp765 > 4u?*(( int*)
_temp765) == Cyc_Absyn_PointerType: 0){ _LL772: _temp771=(( struct Cyc_Absyn_PointerType_struct*)
_temp765)->f1; _LL782: _temp781=( void*) _temp771.elt_typ; goto _LL780; _LL780:
_temp779=( void*) _temp771.rgn_typ; goto _LL778; _LL778: _temp777= _temp771.nullable;
goto _LL776; _LL776: _temp775= _temp771.tq; goto _LL774; _LL774: _temp773=
_temp771.bounds; goto _LL768;} else{ goto _LL769;} _LL769: goto _LL770; _LL768:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp779); Cyc_Tcutil_check_nonzero_bound(
loc, _temp773);{ void* _temp783= Cyc_Tcutil_compress( _temp781); struct Cyc_Absyn_FnInfo
_temp789; struct Cyc_List_List* _temp791; struct Cyc_List_List* _temp793; struct
Cyc_Absyn_VarargInfo* _temp795; int _temp797; struct Cyc_List_List* _temp799;
void* _temp801; struct Cyc_Core_Opt* _temp803; struct Cyc_List_List* _temp805;
_LL785: if(( unsigned int) _temp783 > 4u?*(( int*) _temp783) == Cyc_Absyn_FnType:
0){ _LL790: _temp789=(( struct Cyc_Absyn_FnType_struct*) _temp783)->f1; _LL806:
_temp805= _temp789.tvars; goto _LL804; _LL804: _temp803= _temp789.effect; goto
_LL802; _LL802: _temp801=( void*) _temp789.ret_typ; goto _LL800; _LL800:
_temp799= _temp789.args; goto _LL798; _LL798: _temp797= _temp789.c_varargs; goto
_LL796; _LL796: _temp795= _temp789.cyc_varargs; goto _LL794; _LL794: _temp793=
_temp789.rgn_po; goto _LL792; _LL792: _temp791= _temp789.attributes; goto _LL786;}
else{ goto _LL787;} _LL787: goto _LL788; _LL786: if( topt != 0){ Cyc_Tcutil_unify(
_temp801,*(( void**) _check_null( topt)));} while( es != 0? _temp799 != 0: 0) {
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp799))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp807)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp812= _tag_arr("actual argument has type %s but formal has type %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp814;
_temp814.tag= Cyc_Stdio_String_pa; _temp814.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp810=(
void*)& _temp814; struct Cyc_Stdio_String_pa_struct _temp813; _temp813.tag= Cyc_Stdio_String_pa;
_temp813.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp811=(
void*)& _temp813; void* _temp808[ 2u]={ _temp810, _temp811}; struct _tagged_arr
_temp809={( void*) _temp808,( void*) _temp808,( void*)( _temp808 + 2u)};
_temp807( _temp812, _temp809);}}}));} es=(( struct Cyc_List_List*) _check_null(
es))->tl; _temp799=(( struct Cyc_List_List*) _check_null( _temp799))->tl;} if(
_temp799 != 0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u));} else{ if(( es != 0? 1: _temp797)? 1: _temp795 !=
0){ if( _temp797){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd);}} else{ if( _temp795 == 0){ Cyc_Tcutil_terr( loc,
_tag_arr("too many arguments for function", sizeof( unsigned char), 32u));}
else{ int _temp817; void* _temp819; void* _temp821; struct Cyc_Absyn_VarargInfo
_temp815=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp795)); _LL822:
_temp821=( void*) _temp815.type; goto _LL820; _LL820: _temp819=( void*) _temp815.rgn;
goto _LL818; _LL818: _temp817= _temp815.inject; goto _LL816; _LL816: { struct
Cyc_Absyn_VarargCallInfo* _temp823=({ struct Cyc_Absyn_VarargCallInfo* _temp871=(
struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp871->num_varargs= 0; _temp871->injectors= 0; _temp871->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp795); _temp871;});* vararg_call_info= _temp823; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp819)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp824)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp829= _tag_arr("I have no idea how this could fail, but it did because the region for the varargs (%s) doesn't unify with the expected new_block region (%s).",
sizeof( unsigned char), 142u); struct Cyc_Stdio_String_pa_struct _temp831;
_temp831.tag= Cyc_Stdio_String_pa; _temp831.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp819);{ void* _temp827=( void*)& _temp831; struct Cyc_Stdio_String_pa_struct
_temp830; _temp830.tag= Cyc_Stdio_String_pa; _temp830.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));{ void* _temp828=( void*)&
_temp830; void* _temp825[ 2u]={ _temp827, _temp828}; struct _tagged_arr _temp826={(
void*) _temp825,( void*) _temp825,( void*)( _temp825 + 2u)}; _temp824( _temp829,
_temp826);}}}));} if( ! _temp817){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp821,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp821)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp832)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp837= _tag_arr("vararg requires type %s but argument has type %s",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp839;
_temp839.tag= Cyc_Stdio_String_pa; _temp839.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp821);{ void* _temp835=( void*)& _temp839; struct Cyc_Stdio_String_pa_struct
_temp838; _temp838.tag= Cyc_Stdio_String_pa; _temp838.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
void* _temp836=( void*)& _temp838; void* _temp833[ 2u]={ _temp835, _temp836};
struct _tagged_arr _temp834={( void*) _temp833,( void*) _temp833,( void*)(
_temp833 + 2u)}; _temp832( _temp837, _temp834);}}}));}}} else{ void* _temp840=
Cyc_Tcutil_compress( _temp821); struct Cyc_Absyn_TunionInfo _temp846; void*
_temp848; struct Cyc_List_List* _temp850; void* _temp852; struct Cyc_Absyn_Tuniondecl*
_temp854; _LL842: if(( unsigned int) _temp840 > 4u?*(( int*) _temp840) == Cyc_Absyn_TunionType:
0){ _LL847: _temp846=(( struct Cyc_Absyn_TunionType_struct*) _temp840)->f1;
_LL853: _temp852=( void*) _temp846.tunion_info; if(*(( int*) _temp852) == Cyc_Absyn_KnownTunion){
_LL855: _temp854=(( struct Cyc_Absyn_KnownTunion_struct*) _temp852)->f1; goto
_LL851;} else{ goto _LL844;} _LL851: _temp850= _temp846.targs; goto _LL849;
_LL849: _temp848=( void*) _temp846.rgn; goto _LL843;} else{ goto _LL844;} _LL844:
goto _LL845; _LL843: { struct Cyc_Absyn_Tuniondecl* _temp856=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp854->name); if( ! Cyc_Tcutil_unify( _temp848, _temp819)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp857)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp860=
_tag_arr("bad region for injected varargs", sizeof( unsigned char), 32u); void*
_temp858[ 0u]={}; struct _tagged_arr _temp859={( void*) _temp858,( void*)
_temp858,( void*)( _temp858 + 0u)}; _temp857( _temp860, _temp859);}));}{ struct
Cyc_List_List* fields= 0; if( _temp856->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp861)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp865= _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp866;
_temp866.tag= Cyc_Stdio_String_pa; _temp866.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp821);{ void* _temp864=( void*)& _temp866; void* _temp862[ 1u]={ _temp864};
struct _tagged_arr _temp863={( void*) _temp862,( void*) _temp862,( void*)(
_temp862 + 1u)}; _temp861( _temp865, _temp863);}}));} else{ fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp856->fields))->v;}{ struct _RegionHandle
_temp867= _new_region(); struct _RegionHandle* rgn=& _temp867; _push_region( rgn);{
struct Cyc_List_List* _temp868=(( struct Cyc_List_List*(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y))
Cyc_List_rzip)( rgn, rgn, _temp856->tvs, _temp850); for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp869= Cyc_Tcexp_tcInjection( te, e1, _temp821,
rgn, _temp868, fields); if( _temp869 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->injectors,({ struct Cyc_List_List* _temp870=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp870->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp869)); _temp870->tl= 0;
_temp870;}));}}}}}; _pop_region( rgn);} goto _LL841;}} _LL845: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL841; _LL841:;}}}}}} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp803))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp793); return _temp801; _LL788: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL784:;}
_LL770: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL766:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp872)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp876= _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp877;
_temp877.tag= Cyc_Stdio_String_pa; _temp877.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp875=(
void*)& _temp877; void* _temp873[ 1u]={ _temp875}; struct _tagged_arr _temp874={(
void*) _temp873,( void*) _temp873,( void*)( _temp873 + 1u)}; _temp872( _temp876,
_temp874);}}));} return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp878=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp878->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp878;}));} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp879= t1; struct Cyc_Absyn_PtrInfo _temp885; struct Cyc_Absyn_Conref*
_temp887; struct Cyc_Absyn_Tqual _temp889; struct Cyc_Absyn_Conref* _temp891;
void* _temp893; void* _temp895; _LL881: if(( unsigned int) _temp879 > 4u?*(( int*)
_temp879) == Cyc_Absyn_PointerType: 0){ _LL886: _temp885=(( struct Cyc_Absyn_PointerType_struct*)
_temp879)->f1; _LL896: _temp895=( void*) _temp885.elt_typ; goto _LL894; _LL894:
_temp893=( void*) _temp885.rgn_typ; goto _LL892; _LL892: _temp891= _temp885.nullable;
goto _LL890; _LL890: _temp889= _temp885.tq; goto _LL888; _LL888: _temp887=
_temp885.bounds; goto _LL882;} else{ goto _LL883;} _LL883: goto _LL884; _LL882:{
void* _temp897= Cyc_Tcutil_compress( _temp895); struct Cyc_Absyn_FnInfo _temp903;
struct Cyc_List_List* _temp905; struct Cyc_List_List* _temp907; struct Cyc_Absyn_VarargInfo*
_temp909; int _temp911; struct Cyc_List_List* _temp913; void* _temp915; struct
Cyc_Core_Opt* _temp917; struct Cyc_List_List* _temp919; _LL899: if((
unsigned int) _temp897 > 4u?*(( int*) _temp897) == Cyc_Absyn_FnType: 0){ _LL904:
_temp903=(( struct Cyc_Absyn_FnType_struct*) _temp897)->f1; _LL920: _temp919=
_temp903.tvars; goto _LL918; _LL918: _temp917= _temp903.effect; goto _LL916;
_LL916: _temp915=( void*) _temp903.ret_typ; goto _LL914; _LL914: _temp913=
_temp903.args; goto _LL912; _LL912: _temp911= _temp903.c_varargs; goto _LL910;
_LL910: _temp909= _temp903.cyc_varargs; goto _LL908; _LL908: _temp907= _temp903.rgn_po;
goto _LL906; _LL906: _temp905= _temp903.attributes; goto _LL900;} else{ goto
_LL901;} _LL901: goto _LL902; _LL900: { struct Cyc_List_List* instantiation= 0;
if( Cyc_List_length( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp919)){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("type instantiation mismatch",
sizeof( unsigned char), 28u));} for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, _temp919=(( struct Cyc_List_List*) _check_null( _temp919))->tl)){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp919))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp921=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp921->hd=( void*)({ struct _tuple5* _temp922=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp922->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp919))->hd; _temp922->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp922;}); _temp921->tl=
instantiation; _temp921;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp926=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp926[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp927; _temp927.tag= Cyc_Absyn_FnType; _temp927.f1=({ struct Cyc_Absyn_FnInfo
_temp928; _temp928.tvars= 0; _temp928.effect= _temp917; _temp928.ret_typ=( void*)
_temp915; _temp928.args= _temp913; _temp928.c_varargs= _temp911; _temp928.cyc_varargs=
_temp909; _temp928.rgn_po= _temp907; _temp928.attributes= _temp905; _temp928;});
_temp927;}); _temp926;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp923=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp923[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp924; _temp924.tag= Cyc_Absyn_PointerType;
_temp924.f1=({ struct Cyc_Absyn_PtrInfo _temp925; _temp925.elt_typ=( void*)
new_fn_typ; _temp925.rgn_typ=( void*) _temp893; _temp925.nullable= _temp891;
_temp925.tq= _temp889; _temp925.bounds= _temp887; _temp925;}); _temp924;});
_temp923;}); return new_typ;}} _LL902: goto _LL898; _LL898:;} goto _LL880;
_LL884: goto _LL880; _LL880:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp929)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp933= _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp934;
_temp934.tag= Cyc_Stdio_String_pa; _temp934.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp932=( void*)& _temp934; void* _temp930[ 1u]={ _temp932}; struct
_tagged_arr _temp931={( void*) _temp930,( void*) _temp930,( void*)( _temp930 + 1u)};
_temp929( _temp933, _temp931);}}));}} static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t, struct Cyc_Absyn_Exp*
e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? !
Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp935)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp940= _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp942;
_temp942.tag= Cyc_Stdio_String_pa; _temp942.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp938=( void*)& _temp942; struct Cyc_Stdio_String_pa_struct
_temp941; _temp941.tag= Cyc_Stdio_String_pa; _temp941.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp939=( void*)& _temp941; void* _temp936[
2u]={ _temp938, _temp939}; struct _tagged_arr _temp937={( void*) _temp936,( void*)
_temp936,( void*)( _temp936 + 2u)}; _temp935( _temp940, _temp937);}}}));} return
t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ void** _temp943= 0; struct Cyc_Absyn_Tqual
_temp944= Cyc_Absyn_empty_tqual(); if( topt != 0){ void* _temp945= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp951; struct Cyc_Absyn_Tqual
_temp953; void* _temp955; _LL947: if(( unsigned int) _temp945 > 4u?*(( int*)
_temp945) == Cyc_Absyn_PointerType: 0){ _LL952: _temp951=(( struct Cyc_Absyn_PointerType_struct*)
_temp945)->f1; _LL956: _temp955=( void*) _temp951.elt_typ; goto _LL954; _LL954:
_temp953= _temp951.tq; goto _LL948;} else{ goto _LL949;} _LL949: goto _LL950;
_LL948: _temp943=({ void** _temp957=( void**) GC_malloc( sizeof( void*));
_temp957[ 0]= _temp955; _temp957;}); _temp944= _temp953; goto _LL946; _LL950:
goto _LL946; _LL946:;} Cyc_Tcexp_tcExpNoInst( te, _temp943, e);{ void* _temp958=(
void*) e->r; _LL960: if(*(( int*) _temp958) == Cyc_Absyn_Struct_e){ goto _LL961;}
else{ goto _LL962;} _LL962: if(*(( int*) _temp958) == Cyc_Absyn_Tuple_e){ goto
_LL963;} else{ goto _LL964;} _LL964: goto _LL965; _LL961: goto _LL963; _LL963:
Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof( unsigned char), 19u));{
void*(* _temp966)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp967= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp973; struct Cyc_Absyn_Conref* _temp975;
_LL969: if(( unsigned int) _temp967 > 4u?*(( int*) _temp967) == Cyc_Absyn_PointerType:
0){ _LL974: _temp973=(( struct Cyc_Absyn_PointerType_struct*) _temp967)->f1;
_LL976: _temp975= _temp973.nullable; goto _LL970;} else{ goto _LL971;} _LL971:
goto _LL972; _LL970: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp975)){ _temp966= Cyc_Absyn_star_typ;} goto _LL968; _LL972: goto _LL968;
_LL968:;} return _temp966(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp944);} _LL965: goto _LL959; _LL959:;}{ void*
_temp979; int _temp981; struct _tuple6 _temp977= Cyc_Tcutil_addressof_props( te,
e); _LL982: _temp981= _temp977.f1; goto _LL980; _LL980: _temp979= _temp977.f2;
goto _LL978; _LL978: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp981){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp979, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp983)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp987= _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u); struct Cyc_Stdio_String_pa_struct _temp988;
_temp988.tag= Cyc_Stdio_String_pa; _temp988.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp986=( void*)& _temp988; void* _temp984[ 1u]={ _temp986}; struct
_tagged_arr _temp985={( void*) _temp984,( void*) _temp984,( void*)( _temp984 + 1u)};
_temp983( _temp987, _temp985);}}));} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t);{ int bad_type= 1;{ void* _temp989= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp997; _LL991: if(( unsigned int) _temp989
> 4u?*(( int*) _temp989) == Cyc_Absyn_StructType: 0){ _LL998: _temp997=(( struct
Cyc_Absyn_StructType_struct*) _temp989)->f3; goto _LL992;} else{ goto _LL993;}
_LL993: if(( unsigned int) _temp989 > 4u?*(( int*) _temp989) == Cyc_Absyn_AnonStructType:
0){ goto _LL994;} else{ goto _LL995;} _LL995: goto _LL996; _LL992: if( _temp997
== 0){ return Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u));} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp997)))->fields == 0){ goto _LL990;} goto _LL994; _LL994:
bad_type= 0; goto _LL990; _LL996: goto _LL990; _LL990:;} if( bad_type){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp999)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1003=
_tag_arr("%s is not a known struct type", sizeof( unsigned char), 30u); struct
Cyc_Stdio_String_pa_struct _temp1004; _temp1004.tag= Cyc_Stdio_String_pa;
_temp1004.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1002=(
void*)& _temp1004; void* _temp1000[ 1u]={ _temp1002}; struct _tagged_arr
_temp1001={( void*) _temp1000,( void*) _temp1000,( void*)( _temp1000 + 1u)};
_temp999( _temp1003, _temp1001);}}));} return Cyc_Absyn_uint_t;}} static void*
Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1005= t;
struct Cyc_Absyn_PtrInfo _temp1011; struct Cyc_Absyn_Conref* _temp1013; void*
_temp1015; void* _temp1017; _LL1007: if(( unsigned int) _temp1005 > 4u?*(( int*)
_temp1005) == Cyc_Absyn_PointerType: 0){ _LL1012: _temp1011=(( struct Cyc_Absyn_PointerType_struct*)
_temp1005)->f1; _LL1018: _temp1017=( void*) _temp1011.elt_typ; goto _LL1016;
_LL1016: _temp1015=( void*) _temp1011.rgn_typ; goto _LL1014; _LL1014: _temp1013=
_temp1011.bounds; goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010;
_LL1008: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1015); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1013); return _temp1017; _LL1010: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1019)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1023= _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1024;
_temp1024.tag= Cyc_Stdio_String_pa; _temp1024.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1022=( void*)& _temp1024; void* _temp1020[ 1u]={ _temp1022};
struct _tagged_arr _temp1021={( void*) _temp1020,( void*) _temp1020,( void*)(
_temp1020 + 1u)}; _temp1019( _temp1023, _temp1021);}})); _LL1006:;}} static void*
Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1025= t;
struct Cyc_Absyn_Structdecl** _temp1041; struct Cyc_Absyn_Structdecl* _temp1043;
struct Cyc_List_List* _temp1044; struct _tuple1* _temp1046; struct Cyc_List_List*
_temp1048; struct Cyc_List_List* _temp1050; struct Cyc_Absyn_Uniondecl**
_temp1052; struct Cyc_Absyn_Uniondecl* _temp1054; struct Cyc_List_List*
_temp1055; struct _tuple1* _temp1057; _LL1027: if(( unsigned int) _temp1025 > 4u?*((
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
int*) _temp1025) == Cyc_Absyn_ArrayType: 0){ goto _LL1059;} else{ goto _LL1037;}
_LL1059: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
== 0){ goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int) _temp1025
> 4u?*(( int*) _temp1025) == Cyc_Absyn_PointerType: 0){ goto _LL1060;} else{
goto _LL1039;} _LL1060: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) == 0){ goto _LL1038;} else{ goto _LL1039;} _LL1039: goto
_LL1040; _LL1028: if( _temp1046 == 0){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("unresolved struct type (COMPILER ERROR)", sizeof( unsigned char), 40u));}{
struct Cyc_Absyn_Structfield* _temp1061= Cyc_Absyn_lookup_struct_field(
_temp1043, f); if( _temp1061 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1062)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1067= _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1069;
_temp1069.tag= Cyc_Stdio_String_pa; _temp1069.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1046));{ void* _temp1065=( void*)& _temp1069;
struct Cyc_Stdio_String_pa_struct _temp1068; _temp1068.tag= Cyc_Stdio_String_pa;
_temp1068.f1=( struct _tagged_arr)* f;{ void* _temp1066=( void*)& _temp1068;
void* _temp1063[ 2u]={ _temp1065, _temp1066}; struct _tagged_arr _temp1064={(
void*) _temp1063,( void*) _temp1063,( void*)( _temp1063 + 2u)}; _temp1062(
_temp1067, _temp1064);}}}));}{ void* t2;{ struct _RegionHandle _temp1070=
_new_region(); struct _RegionHandle* rgn=& _temp1070; _push_region( rgn);{
struct Cyc_List_List* _temp1071=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1043->tvs, _temp1044); t2= Cyc_Tcutil_rsubstitute(
rgn, _temp1071,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1061))->type);};
_pop_region( rgn);} return t2;}} _LL1030: { struct Cyc_Absyn_Structfield*
_temp1072= Cyc_Absyn_lookup_field( _temp1048, f); if( _temp1072 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1073)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1077= _tag_arr("struct has no %s field", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1078; _temp1078.tag=
Cyc_Stdio_String_pa; _temp1078.f1=( struct _tagged_arr)* f;{ void* _temp1076=(
void*)& _temp1078; void* _temp1074[ 1u]={ _temp1076}; struct _tagged_arr
_temp1075={( void*) _temp1074,( void*) _temp1074,( void*)( _temp1074 + 1u)};
_temp1073( _temp1077, _temp1075);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1072))->type;} _LL1032: { struct Cyc_Absyn_Structfield*
_temp1079= Cyc_Absyn_lookup_field( _temp1050, f); if( _temp1079 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1080)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1084= _tag_arr("union has no %s field", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1085; _temp1085.tag=
Cyc_Stdio_String_pa; _temp1085.f1=( struct _tagged_arr)* f;{ void* _temp1083=(
void*)& _temp1085; void* _temp1081[ 1u]={ _temp1083}; struct _tagged_arr
_temp1082={( void*) _temp1081,( void*) _temp1081,( void*)( _temp1081 + 1u)};
_temp1080( _temp1084, _temp1082);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1079))->type;} _LL1034: if( _temp1057 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp1086= Cyc_Absyn_lookup_union_field(
_temp1054, f); if( _temp1086 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1087)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1092= _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1094;
_temp1094.tag= Cyc_Stdio_String_pa; _temp1094.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1057));{ void* _temp1090=( void*)& _temp1094;
struct Cyc_Stdio_String_pa_struct _temp1093; _temp1093.tag= Cyc_Stdio_String_pa;
_temp1093.f1=( struct _tagged_arr)* f;{ void* _temp1091=( void*)& _temp1093;
void* _temp1088[ 2u]={ _temp1090, _temp1091}; struct _tagged_arr _temp1089={(
void*) _temp1088,( void*) _temp1088,( void*)( _temp1088 + 2u)}; _temp1087(
_temp1092, _temp1089);}}}));}{ struct Cyc_List_List* _temp1095=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1054->tvs,
_temp1055); void* _temp1096= Cyc_Tcutil_substitute( _temp1095,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1086))->type); return _temp1096;}}
_LL1036:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1097=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1097[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1098; _temp1098.tag= Cyc_Absyn_Primop_e;
_temp1098.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1098.f2=({ struct Cyc_List_List*
_temp1099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1099->hd=( void*) e; _temp1099->tl= 0; _temp1099;}); _temp1098;});
_temp1097;}))); return Cyc_Absyn_uint_t; _LL1038:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1100=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1100[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1101; _temp1101.tag= Cyc_Absyn_Primop_e;
_temp1101.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1101.f2=({ struct Cyc_List_List*
_temp1102=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1102->hd=( void*) e; _temp1102->tl= 0; _temp1102;}); _temp1101;});
_temp1100;}))); return Cyc_Absyn_uint_t; _LL1040: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1103)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1107= _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp1108; _temp1108.tag=
Cyc_Stdio_String_pa; _temp1108.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1106=( void*)& _temp1108; void* _temp1104[ 1u]={ _temp1106};
struct _tagged_arr _temp1105={( void*) _temp1104,( void*) _temp1104,( void*)(
_temp1104 + 1u)}; _temp1103( _temp1107, _temp1105);}}));} else{ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1109)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1113= _tag_arr("expecting struct or union, found %s", sizeof( unsigned char),
36u); struct Cyc_Stdio_String_pa_struct _temp1114; _temp1114.tag= Cyc_Stdio_String_pa;
_temp1114.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1112=(
void*)& _temp1114; void* _temp1110[ 1u]={ _temp1112}; struct _tagged_arr
_temp1111={( void*) _temp1110,( void*) _temp1110,( void*)( _temp1110 + 1u)};
_temp1109( _temp1113, _temp1111);}}));} _LL1026:;}} static void* Cyc_Tcexp_tcStructArrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp1115= t; struct Cyc_Absyn_PtrInfo _temp1121; struct Cyc_Absyn_Conref*
_temp1123; void* _temp1125; void* _temp1127; _LL1117: if(( unsigned int)
_temp1115 > 4u?*(( int*) _temp1115) == Cyc_Absyn_PointerType: 0){ _LL1122:
_temp1121=(( struct Cyc_Absyn_PointerType_struct*) _temp1115)->f1; _LL1128:
_temp1127=( void*) _temp1121.elt_typ; goto _LL1126; _LL1126: _temp1125=( void*)
_temp1121.rgn_typ; goto _LL1124; _LL1124: _temp1123= _temp1121.bounds; goto
_LL1118;} else{ goto _LL1119;} _LL1119: goto _LL1120; _LL1118: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1123);{ void* _temp1129= Cyc_Tcutil_compress( _temp1127); struct Cyc_Absyn_Structdecl**
_temp1141; struct Cyc_Absyn_Structdecl* _temp1143; struct Cyc_List_List*
_temp1144; struct _tuple1* _temp1146; struct Cyc_Absyn_Uniondecl** _temp1148;
struct Cyc_Absyn_Uniondecl* _temp1150; struct Cyc_List_List* _temp1151; struct
_tuple1* _temp1153; struct Cyc_List_List* _temp1155; struct Cyc_List_List*
_temp1157; _LL1131: if(( unsigned int) _temp1129 > 4u?*(( int*) _temp1129) ==
Cyc_Absyn_StructType: 0){ _LL1147: _temp1146=(( struct Cyc_Absyn_StructType_struct*)
_temp1129)->f1; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_StructType_struct*)
_temp1129)->f2; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_StructType_struct*)
_temp1129)->f3; if( _temp1141 == 0){ goto _LL1133;} else{ _temp1143=* _temp1141;
goto _LL1132;}} else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1129 > 4u?*((
int*) _temp1129) == Cyc_Absyn_UnionType: 0){ _LL1154: _temp1153=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f1; goto _LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f2; goto _LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f3; if( _temp1148 == 0){ goto _LL1135;} else{ _temp1150=* _temp1148;
goto _LL1134;}} else{ goto _LL1135;} _LL1135: if(( unsigned int) _temp1129 > 4u?*((
int*) _temp1129) == Cyc_Absyn_AnonStructType: 0){ _LL1156: _temp1155=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1129)->f1; goto _LL1136;} else{ goto
_LL1137;} _LL1137: if(( unsigned int) _temp1129 > 4u?*(( int*) _temp1129) == Cyc_Absyn_AnonUnionType:
0){ _LL1158: _temp1157=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1129)->f1;
goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1132: { struct Cyc_Absyn_Structfield*
_temp1159= Cyc_Absyn_lookup_struct_field( _temp1143, f); if( _temp1159 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1160)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1165= _tag_arr("struct %s has no %s field", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1167; _temp1167.tag=
Cyc_Stdio_String_pa; _temp1167.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1146));{ void* _temp1163=( void*)& _temp1167;
struct Cyc_Stdio_String_pa_struct _temp1166; _temp1166.tag= Cyc_Stdio_String_pa;
_temp1166.f1=( struct _tagged_arr)* f;{ void* _temp1164=( void*)& _temp1166;
void* _temp1161[ 2u]={ _temp1163, _temp1164}; struct _tagged_arr _temp1162={(
void*) _temp1161,( void*) _temp1161,( void*)( _temp1161 + 2u)}; _temp1160(
_temp1165, _temp1162);}}}));}{ void* t3;{ struct _RegionHandle _temp1168=
_new_region(); struct _RegionHandle* rgn=& _temp1168; _push_region( rgn);{
struct Cyc_List_List* _temp1169=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1143->tvs, _temp1144); t3= Cyc_Tcutil_rsubstitute(
rgn, _temp1169,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1159))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1125); return
t3;}} _LL1134: { struct Cyc_Absyn_Structfield* _temp1170= Cyc_Absyn_lookup_union_field(
_temp1150, f); if( _temp1170 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1171)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1176= _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1178;
_temp1178.tag= Cyc_Stdio_String_pa; _temp1178.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1153));{ void* _temp1174=( void*)& _temp1178;
struct Cyc_Stdio_String_pa_struct _temp1177; _temp1177.tag= Cyc_Stdio_String_pa;
_temp1177.f1=( struct _tagged_arr)* f;{ void* _temp1175=( void*)& _temp1177;
void* _temp1172[ 2u]={ _temp1174, _temp1175}; struct _tagged_arr _temp1173={(
void*) _temp1172,( void*) _temp1172,( void*)( _temp1172 + 2u)}; _temp1171(
_temp1176, _temp1173);}}}));}{ struct Cyc_List_List* _temp1179=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1150->tvs,
_temp1151); void* _temp1180= Cyc_Tcutil_substitute( _temp1179,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1170))->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1125); return _temp1180;}} _LL1136: { struct Cyc_Absyn_Structfield*
_temp1181= Cyc_Absyn_lookup_field( _temp1155, f); if( _temp1181 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1182)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1186= _tag_arr("struct has no %s field", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1187; _temp1187.tag=
Cyc_Stdio_String_pa; _temp1187.f1=( struct _tagged_arr)* f;{ void* _temp1185=(
void*)& _temp1187; void* _temp1183[ 1u]={ _temp1185}; struct _tagged_arr
_temp1184={( void*) _temp1183,( void*) _temp1183,( void*)( _temp1183 + 1u)};
_temp1182( _temp1186, _temp1184);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1181))->type;} _LL1138: { struct Cyc_Absyn_Structfield*
_temp1188= Cyc_Absyn_lookup_field( _temp1157, f); if( _temp1188 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1189)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1193= _tag_arr("union has no %s field", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1194; _temp1194.tag=
Cyc_Stdio_String_pa; _temp1194.f1=( struct _tagged_arr)* f;{ void* _temp1192=(
void*)& _temp1194; void* _temp1190[ 1u]={ _temp1192}; struct _tagged_arr
_temp1191={( void*) _temp1190,( void*) _temp1190,( void*)( _temp1190 + 1u)};
_temp1189( _temp1193, _temp1191);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1188))->type;} _LL1140: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1195)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1199= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1200;
_temp1200.tag= Cyc_Stdio_String_pa; _temp1200.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1198=( void*)& _temp1200; void* _temp1196[ 1u]={ _temp1198};
struct _tagged_arr _temp1197={( void*) _temp1196,( void*) _temp1196,( void*)(
_temp1196 + 1u)}; _temp1195( _temp1199, _temp1197);}})); _LL1130:;} _LL1120:
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1201)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1205= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1206;
_temp1206.tag= Cyc_Stdio_String_pa; _temp1206.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1204=( void*)& _temp1206; void* _temp1202[ 1u]={ _temp1204};
struct _tagged_arr _temp1203={( void*) _temp1202,( void*) _temp1202,( void*)(
_temp1202 + 1u)}; _temp1201( _temp1205, _temp1203);}})); _LL1116:;}} static void*
Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1207; _push_handler(& _temp1207);{ int
_temp1209= 0; if( setjmp( _temp1207.handler)){ _temp1209= 1;} if( ! _temp1209){{
void* _temp1210=(*(( struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1210;}; _pop_handler();} else{
void* _temp1208=( void*) _exn_thrown; void* _temp1212= _temp1208; _LL1214: if(
_temp1212 == Cyc_List_Nth){ goto _LL1215;} else{ goto _LL1216;} _LL1216: goto
_LL1217; _LL1215: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp1218)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp1223= _tag_arr("index is %d but tuple has only %d fields",
sizeof( unsigned char), 41u); struct Cyc_Stdio_Int_pa_struct _temp1225;
_temp1225.tag= Cyc_Stdio_Int_pa; _temp1225.f1= i;{ void* _temp1221=( void*)&
_temp1225; struct Cyc_Stdio_Int_pa_struct _temp1224; _temp1224.tag= Cyc_Stdio_Int_pa;
_temp1224.f1=( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ts);{ void* _temp1222=( void*)& _temp1224; void* _temp1219[ 2u]={ _temp1221,
_temp1222}; struct _tagged_arr _temp1220={( void*) _temp1219,( void*) _temp1219,(
void*)( _temp1219 + 2u)}; _temp1218( _temp1223, _temp1220);}}})); _LL1217:( void)
_throw( _temp1212); _LL1213:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1226)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1230= _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp1231;
_temp1231.tag= Cyc_Stdio_String_pa; _temp1231.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp1229=( void*)& _temp1231; void* _temp1227[ 1u]={ _temp1229};
struct _tagged_arr _temp1228={( void*) _temp1227,( void*) _temp1227,( void*)(
_temp1227 + 1u)}; _temp1226( _temp1230, _temp1228);}}));}{ void* _temp1232= t1;
struct Cyc_Absyn_PtrInfo _temp1240; struct Cyc_Absyn_Conref* _temp1242; struct
Cyc_Absyn_Tqual _temp1244; void* _temp1246; void* _temp1248; struct Cyc_List_List*
_temp1250; _LL1234: if(( unsigned int) _temp1232 > 4u?*(( int*) _temp1232) ==
Cyc_Absyn_PointerType: 0){ _LL1241: _temp1240=(( struct Cyc_Absyn_PointerType_struct*)
_temp1232)->f1; _LL1249: _temp1248=( void*) _temp1240.elt_typ; goto _LL1247;
_LL1247: _temp1246=( void*) _temp1240.rgn_typ; goto _LL1245; _LL1245: _temp1244=
_temp1240.tq; goto _LL1243; _LL1243: _temp1242= _temp1240.bounds; goto _LL1235;}
else{ goto _LL1236;} _LL1236: if(( unsigned int) _temp1232 > 4u?*(( int*)
_temp1232) == Cyc_Absyn_TupleType: 0){ _LL1251: _temp1250=(( struct Cyc_Absyn_TupleType_struct*)
_temp1232)->f1; goto _LL1237;} else{ goto _LL1238;} _LL1238: goto _LL1239;
_LL1235: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1242);} else{{ void* _temp1252=( void*)( Cyc_Absyn_compress_conref(
_temp1242))->v; void* _temp1258; struct Cyc_Absyn_Exp* _temp1260; struct Cyc_Absyn_Exp
_temp1262; void* _temp1263; void* _temp1265; int _temp1267; void* _temp1269;
_LL1254: if(( unsigned int) _temp1252 > 1u?*(( int*) _temp1252) == Cyc_Absyn_Eq_constr:
0){ _LL1259: _temp1258=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1252)->f1;
if(( unsigned int) _temp1258 > 1u?*(( int*) _temp1258) == Cyc_Absyn_Upper_b: 0){
_LL1261: _temp1260=(( struct Cyc_Absyn_Upper_b_struct*) _temp1258)->f1;
_temp1262=* _temp1260; _LL1264: _temp1263=( void*) _temp1262.r; if(*(( int*)
_temp1263) == Cyc_Absyn_Const_e){ _LL1266: _temp1265=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1263)->f1; if(( unsigned int) _temp1265 > 1u?*(( int*) _temp1265) == Cyc_Absyn_Int_c:
0){ _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1265)->f1;
if( _temp1269 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1268;} else{ goto _LL1256;}
_LL1268: _temp1267=(( struct Cyc_Absyn_Int_c_struct*) _temp1265)->f2; if(
_temp1267 == 1){ goto _LL1255;} else{ goto _LL1256;}} else{ goto _LL1256;}}
else{ goto _LL1256;}} else{ goto _LL1256;}} else{ goto _LL1256;} _LL1256: goto
_LL1257; _LL1255: Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u)); goto _LL1253; _LL1257: goto _LL1253; _LL1253:;}
Cyc_Tcutil_check_nonzero_bound( loc, _temp1242);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1246); return _temp1248; _LL1237: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1250, e2); _LL1239: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1271)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1275= _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp1276;
_temp1276.tag= Cyc_Stdio_String_pa; _temp1276.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1274=( void*)& _temp1276; void* _temp1272[ 1u]={ _temp1274};
struct _tagged_arr _temp1273={( void*) _temp1272,( void*) _temp1272,( void*)(
_temp1272 + 1u)}; _temp1271( _temp1275, _temp1273);}})); _LL1233:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1277= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1283; _LL1279: if((
unsigned int) _temp1277 > 4u?*(( int*) _temp1277) == Cyc_Absyn_TupleType: 0){
_LL1284: _temp1283=(( struct Cyc_Absyn_TupleType_struct*) _temp1277)->f1; goto
_LL1280;} else{ goto _LL1281;} _LL1281: goto _LL1282; _LL1280: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1283) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc, _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u)); goto _LL1278;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1283=(( struct Cyc_List_List*) _check_null(
_temp1283))->tl)){ void* _temp1285=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1283))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1285,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1286=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1286->hd=( void*)({ struct _tuple8*
_temp1287=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1287->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1283))->hd)).f1;
_temp1287->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1287;});
_temp1286->tl= fields; _temp1286;});} done= 1; goto _LL1278; _LL1282: goto
_LL1278; _LL1278:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1288=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1288->hd=( void*)({ struct _tuple8* _temp1289=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1289->f1= Cyc_Absyn_empty_tqual(); _temp1289->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1289;}); _temp1288->tl=
fields; _temp1288;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1290=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1290[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1291; _temp1291.tag= Cyc_Absyn_TupleType;
_temp1291.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1291;}); _temp1290;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1322=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1322->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1322;})); struct Cyc_Absyn_Const_e_struct*
_temp1292=({ struct Cyc_Absyn_Const_e_struct* _temp1318=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1318[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1319; _temp1319.tag= Cyc_Absyn_Const_e; _temp1319.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1320=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1320[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1321; _temp1321.tag= Cyc_Absyn_Int_c; _temp1321.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1321.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1321;}); _temp1320;})); _temp1319;}); _temp1318;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1292, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1293=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1293->v=( void*) Cyc_Absyn_uint_t; _temp1293;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1316=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1316[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1317; _temp1317.tag= Cyc_Absyn_ArrayType;
_temp1317.f1=( void*) res; _temp1317.f2= Cyc_Absyn_empty_tqual(); _temp1317.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1317;}); _temp1316;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1294)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1298= _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp1299;
_temp1299.tag= Cyc_Stdio_String_pa; _temp1299.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
res);{ void* _temp1297=( void*)& _temp1299; void* _temp1295[ 1u]={ _temp1297};
struct _tagged_arr _temp1296={( void*) _temp1295,( void*) _temp1295,( void*)(
_temp1295 + 1u)}; _temp1294( _temp1298, _temp1296);}}));}{ int offset= 0; for( 0;
des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1300=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp1306; _LL1302: if(*(( int*)
_temp1300) == Cyc_Absyn_FieldName){ goto _LL1303;} else{ goto _LL1304;} _LL1304:
if(*(( int*) _temp1300) == Cyc_Absyn_ArrayElement){ _LL1307: _temp1306=(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1300)->f1; goto _LL1305;} else{ goto
_LL1301;} _LL1303: Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u)); goto _LL1301; _LL1305: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1306);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1306);
if( i != offset){ Cyc_Tcutil_terr( _temp1306->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1308)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1313= _tag_arr("expecting index designator %d but found %d",
sizeof( unsigned char), 43u); struct Cyc_Stdio_Int_pa_struct _temp1315;
_temp1315.tag= Cyc_Stdio_Int_pa; _temp1315.f1=( unsigned int) offset;{ void*
_temp1311=( void*)& _temp1315; struct Cyc_Stdio_Int_pa_struct _temp1314;
_temp1314.tag= Cyc_Stdio_Int_pa; _temp1314.f1= i;{ void* _temp1312=( void*)&
_temp1314; void* _temp1309[ 2u]={ _temp1311, _temp1312}; struct _tagged_arr
_temp1310={( void*) _temp1309,( void*) _temp1309,( void*)( _temp1309 + 2u)};
_temp1308( _temp1313, _temp1310);}}}));} goto _LL1301;} _LL1301:;}}} return
res_t2;}} static void* Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0,
bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1323)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1327= _tag_arr("expecting unsigned int, found %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp1328;
_temp1328.tag= Cyc_Stdio_String_pa; _temp1328.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v);{ void* _temp1326=(
void*)& _temp1328; void* _temp1324[ 1u]={ _temp1326}; struct _tagged_arr
_temp1325={( void*) _temp1324,( void*) _temp1324,( void*)( _temp1324 + 1u)};
_temp1323( _temp1327, _temp1325);}}));} if( !( vd->tq).q_const){(( int(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1329= 0; struct Cyc_Absyn_Tqual*
_temp1330= 0; if( topt != 0){ void* _temp1331= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1339; struct Cyc_Absyn_Exp*
_temp1341; struct Cyc_Absyn_Tqual _temp1343; struct Cyc_Absyn_Tqual* _temp1345;
void* _temp1346; void** _temp1348; _LL1333: if(( unsigned int) _temp1331 > 4u?*((
int*) _temp1331) == Cyc_Absyn_PointerType: 0){ _LL1340: _temp1339=(( struct Cyc_Absyn_PointerType_struct*)
_temp1331)->f1; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(( unsigned int)
_temp1331 > 4u?*(( int*) _temp1331) == Cyc_Absyn_ArrayType: 0){ _LL1347:
_temp1346=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1331)->f1;
_temp1348=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1331)->f1); goto
_LL1344; _LL1344: _temp1343=(( struct Cyc_Absyn_ArrayType_struct*) _temp1331)->f2;
_temp1345=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1331)->f2; goto _LL1342;
_LL1342: _temp1341=(( struct Cyc_Absyn_ArrayType_struct*) _temp1331)->f3; goto
_LL1336;} else{ goto _LL1337;} _LL1337: goto _LL1338; _LL1334: pinfo= _temp1339;
_temp1329=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1330=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1332; _LL1336: _temp1329=( void**) _temp1348; _temp1330=(
struct Cyc_Absyn_Tqual*) _temp1345; goto _LL1332; _LL1338: goto _LL1332; _LL1332:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1329, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1349=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1349[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1350; _temp1350.tag= Cyc_Absyn_ArrayType;
_temp1350.f1=( void*) t; _temp1350.f2= _temp1330 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1330)); _temp1350.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1350;}); _temp1349;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1351; _push_handler(&
_temp1351);{ int _temp1353= 0; if( setjmp( _temp1351.handler)){ _temp1353= 1;}
if( ! _temp1353){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1352=( void*) _exn_thrown; void* _temp1355= _temp1352; _LL1357:
if( _temp1355 == Cyc_Dict_Absent){ goto _LL1358;} else{ goto _LL1359;} _LL1359:
goto _LL1360; _LL1358: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1361)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1365= _tag_arr("unbound struct name %s", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1366; _temp1366.tag=
Cyc_Stdio_String_pa; _temp1366.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1364=( void*)& _temp1366; void* _temp1362[ 1u]={ _temp1364};
struct _tagged_arr _temp1363={( void*) _temp1362,( void*) _temp1362,( void*)(
_temp1362 + 1u)}; _temp1361( _temp1365, _temp1363);}})); return topt != 0?*((
void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1360:( void) _throw(
_temp1355); _LL1356:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1367= _new_region(); struct _RegionHandle* rgn=&
_temp1367; _push_region( rgn);{ struct _tuple4 _temp1368=({ struct _tuple4
_temp1398; _temp1398.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1398.f2= rgn;
_temp1398;}); struct Cyc_List_List* _temp1369=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1368, sd->tvs); struct Cyc_List_List* _temp1370=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1369); struct Cyc_Absyn_StructType_struct*
_temp1371=({ struct Cyc_Absyn_StructType_struct* _temp1395=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1395[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1396; _temp1396.tag= Cyc_Absyn_StructType;
_temp1396.f1=( struct _tuple1*)* tn; _temp1396.f2= _temp1370; _temp1396.f3=({
struct Cyc_Absyn_Structdecl** _temp1397=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1397[ 0]= sd; _temp1397;});
_temp1396;}); _temp1395;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1371,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1372=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1372->v=( void*)
_temp1370; _temp1372;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc, _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u));{ void* _temp1373=( void*) _temp1371;
_npop_handler( 0u); return _temp1373;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1376; struct Cyc_Absyn_Exp* _temp1377; struct Cyc_Absyn_Structfield*
_temp1379; struct _tuple9* _temp1374=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1376=* _temp1374; _LL1380: _temp1379= _temp1376.f1;
goto _LL1378; _LL1378: _temp1377= _temp1376.f2; goto _LL1375; _LL1375: { void*
_temp1381= Cyc_Tcutil_rsubstitute( rgn, _temp1369,( void*) _temp1379->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1381, _temp1377); if( ! Cyc_Tcutil_coerce_arg( te, _temp1377,
_temp1381)){ Cyc_Tcutil_terr( _temp1377->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1382)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1389= _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp1393;
_temp1393.tag= Cyc_Stdio_String_pa; _temp1393.f1=( struct _tagged_arr)*
_temp1379->name;{ void* _temp1385=( void*)& _temp1393; struct Cyc_Stdio_String_pa_struct
_temp1392; _temp1392.tag= Cyc_Stdio_String_pa; _temp1392.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(* tn);{ void* _temp1386=( void*)& _temp1392; struct Cyc_Stdio_String_pa_struct
_temp1391; _temp1391.tag= Cyc_Stdio_String_pa; _temp1391.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp1381);{ void* _temp1387=( void*)& _temp1391; struct
Cyc_Stdio_String_pa_struct _temp1390; _temp1390.tag= Cyc_Stdio_String_pa;
_temp1390.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1377->topt))->v);{ void* _temp1388=( void*)& _temp1390; void*
_temp1383[ 4u]={ _temp1385, _temp1386, _temp1387, _temp1388}; struct _tagged_arr
_temp1384={( void*) _temp1383,( void*) _temp1383,( void*)( _temp1383 + 4u)};
_temp1382( _temp1389, _temp1384);}}}}}));}}}{ void* _temp1394=( void*) _temp1371;
_npop_handler( 0u); return _temp1394;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1399= _new_region(); struct
_RegionHandle* rgn=& _temp1399; _push_region( rgn);{ void* _temp1400= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1406; _LL1402: if(( unsigned int) _temp1400 > 4u?*((
int*) _temp1400) == Cyc_Absyn_AnonStructType: 0){ _LL1407: _temp1406=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1400)->f1; goto _LL1403;} else{ goto
_LL1404;} _LL1404: goto _LL1405; _LL1403: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1406); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1410; struct Cyc_Absyn_Exp*
_temp1411; struct Cyc_Absyn_Structfield* _temp1413; struct _tuple9* _temp1408=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1410=*
_temp1408; _LL1414: _temp1413= _temp1410.f1; goto _LL1412; _LL1412: _temp1411=
_temp1410.f2; goto _LL1409; _LL1409: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1413->type)), _temp1411); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1411,( void*) _temp1413->type)){ Cyc_Tcutil_terr( _temp1411->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1415)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1421= _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1424;
_temp1424.tag= Cyc_Stdio_String_pa; _temp1424.f1=( struct _tagged_arr)*
_temp1413->name;{ void* _temp1418=( void*)& _temp1424; struct Cyc_Stdio_String_pa_struct
_temp1423; _temp1423.tag= Cyc_Stdio_String_pa; _temp1423.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) _temp1413->type);{ void* _temp1419=( void*)&
_temp1423; struct Cyc_Stdio_String_pa_struct _temp1422; _temp1422.tag= Cyc_Stdio_String_pa;
_temp1422.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1411->topt))->v);{ void* _temp1420=( void*)& _temp1422; void*
_temp1416[ 3u]={ _temp1418, _temp1419, _temp1420}; struct _tagged_arr _temp1417={(
void*) _temp1416,( void*) _temp1416,( void*)( _temp1416 + 3u)}; _temp1415(
_temp1421, _temp1417);}}}}));}} goto _LL1401;} _LL1405:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1401; _LL1401:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1425=
_new_region(); struct _RegionHandle* rgn=& _temp1425; _push_region( rgn);{
struct _tuple4 _temp1426=({ struct _tuple4 _temp1485; _temp1485.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1485.f2= rgn; _temp1485;}); struct Cyc_List_List* _temp1427=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1426, tud->tvs); struct Cyc_List_List*
_temp1428=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1426, tuf->tvs);
struct Cyc_List_List* _temp1429=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1427); struct Cyc_List_List* _temp1430=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1428); struct Cyc_List_List* _temp1431=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1427, _temp1428);* all_ref=({
struct Cyc_Core_Opt* _temp1432=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1432->v=( void*) _temp1429; _temp1432;});* exist_ref=({
struct Cyc_Core_Opt* _temp1433=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1433->v=( void*) _temp1430; _temp1433;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1480=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1480[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1481; _temp1481.tag= Cyc_Absyn_TunionFieldType;
_temp1481.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1482; _temp1482.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1483=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1483[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1484; _temp1484.tag=
Cyc_Absyn_KnownTunionfield; _temp1484.f1= tud; _temp1484.f2= tuf; _temp1484;});
_temp1483;})); _temp1482.targs= _temp1429; _temp1482;}); _temp1481;}); _temp1480;});
if( topt != 0){ void* _temp1434= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionInfo _temp1442; void* _temp1444; struct Cyc_List_List*
_temp1446; _LL1436: if(( unsigned int) _temp1434 > 4u?*(( int*) _temp1434) ==
Cyc_Absyn_TunionFieldType: 0){ goto _LL1437;} else{ goto _LL1438;} _LL1438: if((
unsigned int) _temp1434 > 4u?*(( int*) _temp1434) == Cyc_Absyn_TunionType: 0){
_LL1443: _temp1442=(( struct Cyc_Absyn_TunionType_struct*) _temp1434)->f1;
_LL1447: _temp1446= _temp1442.targs; goto _LL1445; _LL1445: _temp1444=( void*)
_temp1442.rgn; goto _LL1439;} else{ goto _LL1440;} _LL1440: goto _LL1441;
_LL1437: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1435;
_LL1439:{ struct Cyc_List_List* a= _temp1429; for( 0; a != 0? _temp1446 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1446=(( struct Cyc_List_List*)
_check_null( _temp1446))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1446))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1448=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1448->v=(
void*) res; _temp1448;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1449=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1449[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1450; _temp1450.tag=
Cyc_Absyn_TunionType; _temp1450.f1=({ struct Cyc_Absyn_TunionInfo _temp1451;
_temp1451.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1452=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1452[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1453; _temp1453.tag=
Cyc_Absyn_KnownTunion; _temp1453.f1= tud; _temp1453;}); _temp1452;})); _temp1451.targs=
_temp1429; _temp1451.rgn=( void*) _temp1444; _temp1451;}); _temp1450;});
_temp1449;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1454= res;
_npop_handler( 0u); return _temp1454;}} goto _LL1435; _LL1441: goto _LL1435;
_LL1435:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1431,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1455)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1461= _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u); struct Cyc_Stdio_String_pa_struct _temp1464;
_temp1464.tag= Cyc_Stdio_String_pa; _temp1464.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1458=( void*)& _temp1464; struct Cyc_Stdio_String_pa_struct
_temp1463; _temp1463.tag= Cyc_Stdio_String_pa; _temp1463.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1459=( void*)& _temp1463; struct Cyc_Stdio_String_pa_struct
_temp1462; _temp1462.tag= Cyc_Stdio_String_pa; _temp1462.f1=( struct _tagged_arr)(
e->topt == 0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1460=(
void*)& _temp1462; void* _temp1456[ 3u]={ _temp1458, _temp1459, _temp1460};
struct _tagged_arr _temp1457={( void*) _temp1456,( void*) _temp1456,( void*)(
_temp1456 + 3u)}; _temp1455( _temp1461, _temp1457);}}}}));}} if( es != 0){ void*
_temp1471= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1465)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1469= _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp1470;
_temp1470.tag= Cyc_Stdio_String_pa; _temp1470.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1468=( void*)& _temp1470; void* _temp1466[ 1u]={
_temp1468}; struct _tagged_arr _temp1467={( void*) _temp1466,( void*) _temp1466,(
void*)( _temp1466 + 1u)}; _temp1465( _temp1469, _temp1467);}})); _npop_handler(
0u); return _temp1471;} if( ts != 0){ void* _temp1478= Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1472)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1476=
_tag_arr("too few arguments for tunion constructor %s", sizeof( unsigned char),
44u); struct Cyc_Stdio_String_pa_struct _temp1477; _temp1477.tag= Cyc_Stdio_String_pa;
_temp1477.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1475=( void*)& _temp1477; void* _temp1473[ 1u]={ _temp1475}; struct
_tagged_arr _temp1474={( void*) _temp1473,( void*) _temp1473,( void*)( _temp1473
+ 1u)}; _temp1472( _temp1476, _temp1474);}})); _npop_handler( 0u); return
_temp1478;}{ void* _temp1479= res; _npop_handler( 0u); return _temp1479;}}}};
_pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt, void*
t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1500=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1500[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1501; _temp1501.tag= Cyc_Absyn_RgnHandleType;
_temp1501.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1502=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1502->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1502;}));
_temp1501;}); _temp1500;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1486= Cyc_Tcutil_compress(
handle_type); void* _temp1492; _LL1488: if(( unsigned int) _temp1486 > 4u?*((
int*) _temp1486) == Cyc_Absyn_RgnHandleType: 0){ _LL1493: _temp1492=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1486)->f1; goto _LL1489;} else{
goto _LL1490;} _LL1490: goto _LL1491; _LL1489: rgn= _temp1492; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1487; _LL1491: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1494)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1498= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1499; _temp1499.tag=
Cyc_Stdio_String_pa; _temp1499.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1497=( void*)& _temp1499; void* _temp1495[ 1u]={
_temp1497}; struct _tagged_arr _temp1496={( void*) _temp1495,( void*) _temp1495,(
void*)( _temp1495 + 1u)}; _temp1494( _temp1498, _temp1496);}})); goto _LL1487;
_LL1487:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1503= t; struct Cyc_Absyn_TunionFieldInfo
_temp1509; void* _temp1511; struct Cyc_Absyn_Tunionfield* _temp1513; _LL1505:
if(( unsigned int) _temp1503 > 4u?*(( int*) _temp1503) == Cyc_Absyn_TunionFieldType:
0){ _LL1510: _temp1509=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1503)->f1;
_LL1512: _temp1511=( void*) _temp1509.field_info; if(*(( int*) _temp1511) == Cyc_Absyn_KnownTunionfield){
_LL1514: _temp1513=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1511)->f2;
goto _LL1506;} else{ goto _LL1507;}} else{ goto _LL1507;} _LL1507: goto _LL1508;
_LL1506: if( _temp1513->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1504; _LL1508: goto _LL1504; _LL1504:;}{
void*(* _temp1515)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1516= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1522; struct Cyc_Absyn_Conref* _temp1524;
_LL1518: if(( unsigned int) _temp1516 > 4u?*(( int*) _temp1516) == Cyc_Absyn_PointerType:
0){ _LL1523: _temp1522=(( struct Cyc_Absyn_PointerType_struct*) _temp1516)->f1;
_LL1525: _temp1524= _temp1522.nullable; goto _LL1519;} else{ goto _LL1520;}
_LL1520: goto _LL1521; _LL1519: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1524)){ _temp1515= Cyc_Absyn_star_typ;} goto _LL1517; _LL1521: goto _LL1517;
_LL1517:;} return _temp1515( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1526=( void*) s->r; struct Cyc_Absyn_Exp* _temp1536;
struct Cyc_Absyn_Stmt* _temp1538; struct Cyc_Absyn_Stmt* _temp1540; struct Cyc_Absyn_Stmt*
_temp1542; struct Cyc_Absyn_Decl* _temp1544; _LL1528: if(( unsigned int)
_temp1526 > 1u?*(( int*) _temp1526) == Cyc_Absyn_Exp_s: 0){ _LL1537: _temp1536=((
struct Cyc_Absyn_Exp_s_struct*) _temp1526)->f1; goto _LL1529;} else{ goto
_LL1530;} _LL1530: if(( unsigned int) _temp1526 > 1u?*(( int*) _temp1526) == Cyc_Absyn_Seq_s:
0){ _LL1541: _temp1540=(( struct Cyc_Absyn_Seq_s_struct*) _temp1526)->f1; goto
_LL1539; _LL1539: _temp1538=(( struct Cyc_Absyn_Seq_s_struct*) _temp1526)->f2;
goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( unsigned int) _temp1526 > 1u?*((
int*) _temp1526) == Cyc_Absyn_Decl_s: 0){ _LL1545: _temp1544=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1526)->f1; goto _LL1543; _LL1543: _temp1542=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1526)->f2; goto _LL1533;} else{ goto _LL1534;} _LL1534: goto _LL1535;
_LL1529: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1536->topt))->v;
_LL1531: s= _temp1538; continue; _LL1533: s= _temp1542; continue; _LL1535:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1527:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1560=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1560[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1561; _temp1561.tag= Cyc_Absyn_RgnHandleType; _temp1561.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1562=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1562->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1562;})); _temp1561;}); _temp1560;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1546= Cyc_Tcutil_compress( handle_type);
void* _temp1552; _LL1548: if(( unsigned int) _temp1546 > 4u?*(( int*) _temp1546)
== Cyc_Absyn_RgnHandleType: 0){ _LL1553: _temp1552=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1546)->f1; goto _LL1549;} else{ goto _LL1550;} _LL1550: goto _LL1551;
_LL1549: rgn= _temp1552; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1547; _LL1551: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1554)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1558= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1559; _temp1559.tag=
Cyc_Stdio_String_pa; _temp1559.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1557=( void*)& _temp1559; void* _temp1555[ 1u]={
_temp1557}; struct _tagged_arr _temp1556={( void*) _temp1555,( void*) _temp1555,(
void*)( _temp1555 + 1u)}; _temp1554( _temp1558, _temp1556);}})); goto _LL1547;
_LL1547:;}{ void* _temp1563=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1575;
struct Cyc_Absyn_Exp* _temp1577; struct Cyc_Absyn_Vardecl* _temp1579; struct Cyc_List_List*
_temp1581; struct Cyc_Core_Opt* _temp1583; struct Cyc_List_List* _temp1585; void*
_temp1587; struct _tagged_arr _temp1589; _LL1565: if(*(( int*) _temp1563) == Cyc_Absyn_Comprehension_e){
_LL1580: _temp1579=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1563)->f1;
goto _LL1578; _LL1578: _temp1577=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1563)->f2; goto _LL1576; _LL1576: _temp1575=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1563)->f3; goto _LL1566;} else{ goto _LL1567;} _LL1567: if(*(( int*)
_temp1563) == Cyc_Absyn_UnresolvedMem_e){ _LL1584: _temp1583=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1563)->f1; goto _LL1582; _LL1582: _temp1581=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1563)->f2; goto _LL1568;} else{ goto _LL1569;} _LL1569: if(*(( int*)
_temp1563) == Cyc_Absyn_Array_e){ _LL1586: _temp1585=(( struct Cyc_Absyn_Array_e_struct*)
_temp1563)->f1; goto _LL1570;} else{ goto _LL1571;} _LL1571: if(*(( int*)
_temp1563) == Cyc_Absyn_Const_e){ _LL1588: _temp1587=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1563)->f1; if(( unsigned int) _temp1587 > 1u?*(( int*) _temp1587) == Cyc_Absyn_String_c:
0){ _LL1590: _temp1589=(( struct Cyc_Absyn_String_c_struct*) _temp1587)->f1;
goto _LL1572;} else{ goto _LL1573;}} else{ goto _LL1573;} _LL1573: goto _LL1574;
_LL1566: { void* _temp1591= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1592= Cyc_Tcutil_compress( _temp1591); struct Cyc_Absyn_Tqual _temp1598;
void* _temp1600; _LL1594: if(( unsigned int) _temp1592 > 4u?*(( int*) _temp1592)
== Cyc_Absyn_ArrayType: 0){ _LL1601: _temp1600=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1592)->f1; goto _LL1599; _LL1599: _temp1598=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1592)->f2; goto _LL1595;} else{ goto _LL1596;} _LL1596: goto _LL1597;
_LL1595: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1577)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1606=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1606[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1607; _temp1607.tag= Cyc_Absyn_Upper_b; _temp1607.f1=
_temp1577; _temp1607;}); _temp1606;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1603=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1603[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1604; _temp1604.tag= Cyc_Absyn_PointerType;
_temp1604.f1=({ struct Cyc_Absyn_PtrInfo _temp1605; _temp1605.elt_typ=( void*)
_temp1600; _temp1605.rgn_typ=( void*) rgn; _temp1605.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1605.tq= _temp1598; _temp1605.bounds= Cyc_Absyn_new_conref(
b); _temp1605;}); _temp1604;}); _temp1603;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1602=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1602->v=( void*) res_typ; _temp1602;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1597: return Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1593:;} _LL1568:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1608=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1608[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1609; _temp1609.tag= Cyc_Absyn_Array_e; _temp1609.f1=
_temp1581; _temp1609;}); _temp1608;}))); _temp1585= _temp1581; goto _LL1570;
_LL1570: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1610= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1616; struct Cyc_Absyn_Tqual
_temp1618; void* _temp1620; void** _temp1622; _LL1612: if(( unsigned int)
_temp1610 > 4u?*(( int*) _temp1610) == Cyc_Absyn_PointerType: 0){ _LL1617:
_temp1616=(( struct Cyc_Absyn_PointerType_struct*) _temp1610)->f1; _LL1621:
_temp1620=( void*) _temp1616.elt_typ; _temp1622=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1610)->f1).elt_typ; goto _LL1619; _LL1619: _temp1618= _temp1616.tq; goto
_LL1613;} else{ goto _LL1614;} _LL1614: goto _LL1615; _LL1613: elt_typ_opt=(
void**) _temp1622; goto _LL1611; _LL1615: goto _LL1611; _LL1611:;}{ void*
_temp1623= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1624= Cyc_Tcutil_compress( _temp1623); struct Cyc_Absyn_Exp* _temp1630;
struct Cyc_Absyn_Tqual _temp1632; void* _temp1634; _LL1626: if(( unsigned int)
_temp1624 > 4u?*(( int*) _temp1624) == Cyc_Absyn_ArrayType: 0){ _LL1635:
_temp1634=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1624)->f1; goto
_LL1633; _LL1633: _temp1632=(( struct Cyc_Absyn_ArrayType_struct*) _temp1624)->f2;
goto _LL1631; _LL1631: _temp1630=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1624)->f3; goto _LL1627;} else{ goto _LL1628;} _LL1628: goto _LL1629;
_LL1627: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1636=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1636[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1637; _temp1637.tag=
Cyc_Absyn_PointerType; _temp1637.f1=({ struct Cyc_Absyn_PtrInfo _temp1638;
_temp1638.elt_typ=( void*) _temp1634; _temp1638.rgn_typ=( void*) rgn; _temp1638.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1638.tq=
_temp1632; _temp1638.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1639=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1639[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1640; _temp1640.tag= Cyc_Absyn_Upper_b;
_temp1640.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1630); _temp1640;});
_temp1639;})); _temp1638;}); _temp1637;}); _temp1636;}); if( topt != 0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1641=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1641->v=( void*) res_typ; _temp1641;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1625; _LL1629: return Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u)); _LL1625:;} return res_typ;}} _LL1572: { void*
_temp1642= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),(
void*) Cyc_Absyn_Unknown_b); void* _temp1643= Cyc_Tcexp_tcExp( te,( void**)&
_temp1642, e1); return Cyc_Absyn_atb_typ( _temp1643, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1644=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1644[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1645; _temp1645.tag= Cyc_Absyn_Upper_b; _temp1645.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1645;}); _temp1644;}));} _LL1574: { void** topt2=
0; if( topt != 0){ void* _temp1646= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1654; struct Cyc_Absyn_Conref* _temp1656;
void* _temp1658; void** _temp1660; _LL1648: if(( unsigned int) _temp1646 > 4u?*((
int*) _temp1646) == Cyc_Absyn_PointerType: 0){ _LL1655: _temp1654=(( struct Cyc_Absyn_PointerType_struct*)
_temp1646)->f1; _LL1659: _temp1658=( void*) _temp1654.elt_typ; _temp1660=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1646)->f1).elt_typ; goto _LL1657;
_LL1657: _temp1656= _temp1654.nullable; goto _LL1649;} else{ goto _LL1650;}
_LL1650: if(( unsigned int) _temp1646 > 4u?*(( int*) _temp1646) == Cyc_Absyn_TunionType:
0){ goto _LL1651;} else{ goto _LL1652;} _LL1652: goto _LL1653; _LL1649: topt2=(
void**) _temp1660; goto _LL1647; _LL1651: topt2=({ void** _temp1661=( void**)
GC_malloc( sizeof( void*)); _temp1661[ 0]=*(( void**) _check_null( topt));
_temp1661;}); goto _LL1647; _LL1653: goto _LL1647; _LL1647:;}{ void* _temp1662=
Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1664=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1664[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1665; _temp1665.tag=
Cyc_Absyn_PointerType; _temp1665.f1=({ struct Cyc_Absyn_PtrInfo _temp1666;
_temp1666.elt_typ=( void*) _temp1662; _temp1666.rgn_typ=( void*) rgn; _temp1666.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1666.tq= Cyc_Absyn_empty_tqual();
_temp1666.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1667=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1667[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1668; _temp1668.tag= Cyc_Absyn_Upper_b;
_temp1668.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1668;}); _temp1667;})); _temp1666;});
_temp1665;}); _temp1664;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1663=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1663->v=(
void*) res_typ; _temp1663;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1564:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t=
Cyc_Tcutil_compress( t);{ void* _temp1669= t; struct Cyc_Absyn_Exp* _temp1675;
struct Cyc_Absyn_Tqual _temp1677; void* _temp1679; _LL1671: if(( unsigned int)
_temp1669 > 4u?*(( int*) _temp1669) == Cyc_Absyn_ArrayType: 0){ _LL1680:
_temp1679=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1669)->f1; goto
_LL1678; _LL1678: _temp1677=(( struct Cyc_Absyn_ArrayType_struct*) _temp1669)->f2;
goto _LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1669)->f3; goto _LL1672;} else{ goto _LL1673;} _LL1673: goto _LL1674;
_LL1672: { void* _temp1683; struct _tuple6 _temp1681= Cyc_Tcutil_addressof_props(
te, e); _LL1684: _temp1683= _temp1681.f2; goto _LL1682; _LL1682: { void*
_temp1685= _temp1675 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1686=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1686[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1687; _temp1687.tag= Cyc_Absyn_Upper_b;
_temp1687.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1675); _temp1687;});
_temp1686;}); t= Cyc_Absyn_atb_typ( _temp1679, _temp1683, _temp1677, _temp1685);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1674: return t; _LL1670:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1688=( void*) e->r; void* _temp1698; _LL1690: if(*(( int*)
_temp1688) == Cyc_Absyn_Array_e){ goto _LL1691;} else{ goto _LL1692;} _LL1692:
if(*(( int*) _temp1688) == Cyc_Absyn_Comprehension_e){ goto _LL1693;} else{ goto
_LL1694;} _LL1694: if(*(( int*) _temp1688) == Cyc_Absyn_Const_e){ _LL1699:
_temp1698=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1688)->f1; if((
unsigned int) _temp1698 > 1u?*(( int*) _temp1698) == Cyc_Absyn_String_c: 0){
goto _LL1695;} else{ goto _LL1696;}} else{ goto _LL1696;} _LL1696: goto _LL1697;
_LL1691: return t; _LL1693: return t; _LL1695: return t; _LL1697: t= Cyc_Tcutil_compress(
t);{ void* _temp1700= t; struct Cyc_Absyn_Exp* _temp1706; struct Cyc_Absyn_Tqual
_temp1708; void* _temp1710; _LL1702: if(( unsigned int) _temp1700 > 4u?*(( int*)
_temp1700) == Cyc_Absyn_ArrayType: 0){ _LL1711: _temp1710=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f1; goto _LL1709; _LL1709: _temp1708=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f2; goto _LL1707; _LL1707: _temp1706=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f3; goto _LL1703;} else{ goto _LL1704;} _LL1704: goto _LL1705;
_LL1703: { void* _temp1714; struct _tuple6 _temp1712= Cyc_Tcutil_addressof_props(
te, e); _LL1715: _temp1714= _temp1712.f2; goto _LL1713; _LL1713: { void* b=
_temp1706 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1716=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1716[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1717; _temp1717.tag= Cyc_Absyn_Upper_b;
_temp1717.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1706); _temp1717;});
_temp1716;}); t= Cyc_Absyn_atb_typ( _temp1710, _temp1714, _temp1708, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1705: return t; _LL1701:;} _LL1689:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1718=( void*) e->r; struct Cyc_Absyn_Exp* _temp1724; _LL1720: if(*(( int*)
_temp1718) == Cyc_Absyn_NoInstantiate_e){ _LL1725: _temp1724=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1718)->f1; goto _LL1721;} else{ goto _LL1722;} _LL1722: goto _LL1723;
_LL1721: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1724);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1724->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1724->topt))->v))); e->topt=
_temp1724->topt; goto _LL1719; _LL1723: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1726= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1732; struct Cyc_Absyn_Conref*
_temp1734; struct Cyc_Absyn_Tqual _temp1736; struct Cyc_Absyn_Conref* _temp1738;
void* _temp1740; void* _temp1742; _LL1728: if(( unsigned int) _temp1726 > 4u?*((
int*) _temp1726) == Cyc_Absyn_PointerType: 0){ _LL1733: _temp1732=(( struct Cyc_Absyn_PointerType_struct*)
_temp1726)->f1; _LL1743: _temp1742=( void*) _temp1732.elt_typ; goto _LL1741;
_LL1741: _temp1740=( void*) _temp1732.rgn_typ; goto _LL1739; _LL1739: _temp1738=
_temp1732.nullable; goto _LL1737; _LL1737: _temp1736= _temp1732.tq; goto _LL1735;
_LL1735: _temp1734= _temp1732.bounds; goto _LL1729;} else{ goto _LL1730;}
_LL1730: goto _LL1731; _LL1729:{ void* _temp1744= Cyc_Tcutil_compress( _temp1742);
struct Cyc_Absyn_FnInfo _temp1750; struct Cyc_List_List* _temp1752; struct Cyc_List_List*
_temp1754; struct Cyc_Absyn_VarargInfo* _temp1756; int _temp1758; struct Cyc_List_List*
_temp1760; void* _temp1762; struct Cyc_Core_Opt* _temp1764; struct Cyc_List_List*
_temp1766; _LL1746: if(( unsigned int) _temp1744 > 4u?*(( int*) _temp1744) ==
Cyc_Absyn_FnType: 0){ _LL1751: _temp1750=(( struct Cyc_Absyn_FnType_struct*)
_temp1744)->f1; _LL1767: _temp1766= _temp1750.tvars; goto _LL1765; _LL1765:
_temp1764= _temp1750.effect; goto _LL1763; _LL1763: _temp1762=( void*) _temp1750.ret_typ;
goto _LL1761; _LL1761: _temp1760= _temp1750.args; goto _LL1759; _LL1759:
_temp1758= _temp1750.c_varargs; goto _LL1757; _LL1757: _temp1756= _temp1750.cyc_varargs;
goto _LL1755; _LL1755: _temp1754= _temp1750.rgn_po; goto _LL1753; _LL1753:
_temp1752= _temp1750.attributes; goto _LL1747;} else{ goto _LL1748;} _LL1748:
goto _LL1749; _LL1747: if( _temp1766 != 0){ struct _RegionHandle _temp1768=
_new_region(); struct _RegionHandle* rgn=& _temp1768; _push_region( rgn);{
struct _tuple4 _temp1769=({ struct _tuple4 _temp1779; _temp1779.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1779.f2= rgn; _temp1779;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1769, _temp1766); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1776=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1776[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1777; _temp1777.tag= Cyc_Absyn_FnType; _temp1777.f1=({ struct Cyc_Absyn_FnInfo
_temp1778; _temp1778.tvars= 0; _temp1778.effect= _temp1764; _temp1778.ret_typ=(
void*) _temp1762; _temp1778.args= _temp1760; _temp1778.c_varargs= _temp1758;
_temp1778.cyc_varargs= _temp1756; _temp1778.rgn_po= _temp1754; _temp1778.attributes=
_temp1752; _temp1778;}); _temp1777;}); _temp1776;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1773=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1773[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1774; _temp1774.tag= Cyc_Absyn_PointerType;
_temp1774.f1=({ struct Cyc_Absyn_PtrInfo _temp1775; _temp1775.elt_typ=( void*)
ftyp; _temp1775.rgn_typ=( void*) _temp1740; _temp1775.nullable= _temp1738;
_temp1775.tq= _temp1736; _temp1775.bounds= _temp1734; _temp1775;}); _temp1774;});
_temp1773;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1770=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1770[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1771; _temp1771.tag= Cyc_Absyn_Instantiate_e;
_temp1771.f1= inner; _temp1771.f2= ts; _temp1771;}); _temp1770;}))); e->topt=({
struct Cyc_Core_Opt* _temp1772=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1772->v=( void*) new_typ; _temp1772;});}; _pop_region( rgn);}
goto _LL1745; _LL1749: goto _LL1745; _LL1745:;} goto _LL1727; _LL1731: goto
_LL1727; _LL1727:;} goto _LL1719; _LL1719:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1780=( void*) e->r; struct Cyc_Absyn_Exp* _temp1854; struct
_tuple1* _temp1856; struct Cyc_List_List* _temp1858; struct Cyc_Absyn_Exp*
_temp1860; struct Cyc_List_List* _temp1862; struct Cyc_Core_Opt* _temp1864; void*
_temp1866; void* _temp1868; struct _tuple1* _temp1870; struct Cyc_List_List*
_temp1872; void* _temp1874; void* _temp1876; struct Cyc_Absyn_Exp* _temp1878;
struct Cyc_Absyn_Exp* _temp1880; struct Cyc_Core_Opt* _temp1882; struct Cyc_Absyn_Exp*
_temp1884; struct Cyc_Absyn_Exp* _temp1886; struct Cyc_Absyn_Exp* _temp1888;
struct Cyc_Absyn_Exp* _temp1890; struct Cyc_Absyn_Exp* _temp1892; struct Cyc_Absyn_Exp*
_temp1894; struct Cyc_Absyn_VarargCallInfo* _temp1896; struct Cyc_Absyn_VarargCallInfo**
_temp1898; struct Cyc_List_List* _temp1899; struct Cyc_Absyn_Exp* _temp1901;
struct Cyc_Absyn_Exp* _temp1903; struct Cyc_List_List* _temp1905; struct Cyc_Absyn_Exp*
_temp1907; struct Cyc_Absyn_Exp* _temp1909; void* _temp1911; struct Cyc_Absyn_Exp*
_temp1913; struct Cyc_Absyn_Exp* _temp1915; struct Cyc_Absyn_Exp* _temp1917;
struct Cyc_Absyn_Exp* _temp1919; void* _temp1921; struct _tagged_arr* _temp1923;
void* _temp1925; struct Cyc_Absyn_Exp* _temp1927; struct _tagged_arr* _temp1929;
struct Cyc_Absyn_Exp* _temp1931; struct _tagged_arr* _temp1933; struct Cyc_Absyn_Exp*
_temp1935; struct Cyc_Absyn_Exp* _temp1937; struct Cyc_Absyn_Exp* _temp1939;
struct Cyc_List_List* _temp1941; struct Cyc_List_List* _temp1943; struct _tuple2*
_temp1945; struct Cyc_List_List* _temp1947; struct Cyc_Absyn_Stmt* _temp1949;
struct Cyc_Absyn_Fndecl* _temp1951; struct Cyc_Absyn_Exp* _temp1953; struct Cyc_Absyn_Exp*
_temp1955; struct Cyc_Absyn_Exp* _temp1957; struct Cyc_Absyn_Vardecl* _temp1959;
struct Cyc_Absyn_Structdecl* _temp1961; struct Cyc_Absyn_Structdecl** _temp1963;
struct Cyc_List_List* _temp1964; struct Cyc_Core_Opt* _temp1966; struct Cyc_Core_Opt**
_temp1968; struct _tuple1* _temp1969; struct _tuple1** _temp1971; struct Cyc_List_List*
_temp1972; void* _temp1974; struct Cyc_Absyn_Tunionfield* _temp1976; struct Cyc_Absyn_Tuniondecl*
_temp1978; struct Cyc_List_List* _temp1980; struct Cyc_Core_Opt* _temp1982;
struct Cyc_Core_Opt** _temp1984; struct Cyc_Core_Opt* _temp1985; struct Cyc_Core_Opt**
_temp1987; struct Cyc_Absyn_Enumfield* _temp1988; struct Cyc_Absyn_Enumdecl*
_temp1990; struct _tuple1* _temp1992; struct _tuple1** _temp1994; void*
_temp1995; struct Cyc_Absyn_Exp* _temp1997; _LL1782: if(*(( int*) _temp1780) ==
Cyc_Absyn_NoInstantiate_e){ _LL1855: _temp1854=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1780)->f1; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(*(( int*)
_temp1780) == Cyc_Absyn_UnknownId_e){ _LL1857: _temp1856=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1780)->f1; goto _LL1785;} else{ goto _LL1786;} _LL1786: if(*(( int*)
_temp1780) == Cyc_Absyn_UnknownCall_e){ _LL1861: _temp1860=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1780)->f1; goto _LL1859; _LL1859: _temp1858=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1780)->f2; goto _LL1787;} else{ goto _LL1788;} _LL1788: if(*(( int*)
_temp1780) == Cyc_Absyn_UnresolvedMem_e){ _LL1865: _temp1864=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1780)->f1; goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1780)->f2; goto _LL1789;} else{ goto _LL1790;} _LL1790: if(*(( int*)
_temp1780) == Cyc_Absyn_Const_e){ _LL1867: _temp1866=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1780)->f1; goto _LL1791;} else{ goto _LL1792;} _LL1792: if(*(( int*)
_temp1780) == Cyc_Absyn_Var_e){ _LL1871: _temp1870=(( struct Cyc_Absyn_Var_e_struct*)
_temp1780)->f1; goto _LL1869; _LL1869: _temp1868=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1780)->f2; goto _LL1793;} else{ goto _LL1794;} _LL1794: if(*(( int*)
_temp1780) == Cyc_Absyn_Primop_e){ _LL1875: _temp1874=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1780)->f1; goto _LL1873; _LL1873: _temp1872=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1780)->f2; goto _LL1795;} else{ goto _LL1796;} _LL1796: if(*(( int*)
_temp1780) == Cyc_Absyn_Increment_e){ _LL1879: _temp1878=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1780)->f1; goto _LL1877; _LL1877: _temp1876=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1780)->f2; goto _LL1797;} else{ goto _LL1798;} _LL1798: if(*(( int*)
_temp1780) == Cyc_Absyn_AssignOp_e){ _LL1885: _temp1884=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1780)->f1; goto _LL1883; _LL1883: _temp1882=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1780)->f2; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1780)->f3; goto _LL1799;} else{ goto _LL1800;} _LL1800: if(*(( int*)
_temp1780) == Cyc_Absyn_Conditional_e){ _LL1891: _temp1890=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1780)->f1; goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1780)->f2; goto _LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1780)->f3; goto _LL1801;} else{ goto _LL1802;} _LL1802: if(*(( int*)
_temp1780) == Cyc_Absyn_SeqExp_e){ _LL1895: _temp1894=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1780)->f1; goto _LL1893; _LL1893: _temp1892=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1780)->f2; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(*(( int*)
_temp1780) == Cyc_Absyn_FnCall_e){ _LL1902: _temp1901=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1780)->f1; goto _LL1900; _LL1900: _temp1899=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1780)->f2; goto _LL1897; _LL1897: _temp1896=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1780)->f3; _temp1898=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1780)->f3;
goto _LL1805;} else{ goto _LL1806;} _LL1806: if(*(( int*) _temp1780) == Cyc_Absyn_Throw_e){
_LL1904: _temp1903=(( struct Cyc_Absyn_Throw_e_struct*) _temp1780)->f1; goto
_LL1807;} else{ goto _LL1808;} _LL1808: if(*(( int*) _temp1780) == Cyc_Absyn_Instantiate_e){
_LL1908: _temp1907=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1780)->f1;
goto _LL1906; _LL1906: _temp1905=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1780)->f2; goto _LL1809;} else{ goto _LL1810;} _LL1810: if(*(( int*)
_temp1780) == Cyc_Absyn_Cast_e){ _LL1912: _temp1911=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1780)->f1; goto _LL1910; _LL1910: _temp1909=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1780)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(*(( int*)
_temp1780) == Cyc_Absyn_Address_e){ _LL1914: _temp1913=(( struct Cyc_Absyn_Address_e_struct*)
_temp1780)->f1; goto _LL1813;} else{ goto _LL1814;} _LL1814: if(*(( int*)
_temp1780) == Cyc_Absyn_New_e){ _LL1918: _temp1917=(( struct Cyc_Absyn_New_e_struct*)
_temp1780)->f1; goto _LL1916; _LL1916: _temp1915=(( struct Cyc_Absyn_New_e_struct*)
_temp1780)->f2; goto _LL1815;} else{ goto _LL1816;} _LL1816: if(*(( int*)
_temp1780) == Cyc_Absyn_Sizeofexp_e){ _LL1920: _temp1919=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1780)->f1; goto _LL1817;} else{ goto _LL1818;} _LL1818: if(*(( int*)
_temp1780) == Cyc_Absyn_Sizeoftyp_e){ _LL1922: _temp1921=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1780)->f1; goto _LL1819;} else{ goto _LL1820;} _LL1820: if(*(( int*)
_temp1780) == Cyc_Absyn_Offsetof_e){ _LL1926: _temp1925=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1780)->f1; goto _LL1924; _LL1924: _temp1923=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1780)->f2; goto _LL1821;} else{ goto _LL1822;} _LL1822: if(*(( int*)
_temp1780) == Cyc_Absyn_Deref_e){ _LL1928: _temp1927=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1780)->f1; goto _LL1823;} else{ goto _LL1824;} _LL1824: if(*(( int*)
_temp1780) == Cyc_Absyn_StructMember_e){ _LL1932: _temp1931=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1780)->f1; goto _LL1930; _LL1930: _temp1929=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1780)->f2; goto _LL1825;} else{ goto _LL1826;} _LL1826: if(*(( int*)
_temp1780) == Cyc_Absyn_StructArrow_e){ _LL1936: _temp1935=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1780)->f1; goto _LL1934; _LL1934: _temp1933=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1780)->f2; goto _LL1827;} else{ goto _LL1828;} _LL1828: if(*(( int*)
_temp1780) == Cyc_Absyn_Subscript_e){ _LL1940: _temp1939=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1780)->f1; goto _LL1938; _LL1938: _temp1937=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1780)->f2; goto _LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*)
_temp1780) == Cyc_Absyn_Tuple_e){ _LL1942: _temp1941=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1780)->f1; goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*)
_temp1780) == Cyc_Absyn_CompoundLit_e){ _LL1946: _temp1945=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1780)->f1; goto _LL1944; _LL1944: _temp1943=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1780)->f2; goto _LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*)
_temp1780) == Cyc_Absyn_Array_e){ _LL1948: _temp1947=(( struct Cyc_Absyn_Array_e_struct*)
_temp1780)->f1; goto _LL1835;} else{ goto _LL1836;} _LL1836: if(*(( int*)
_temp1780) == Cyc_Absyn_StmtExp_e){ _LL1950: _temp1949=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1780)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*)
_temp1780) == Cyc_Absyn_Codegen_e){ _LL1952: _temp1951=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1780)->f1; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1780) == Cyc_Absyn_Fill_e){ _LL1954: _temp1953=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1780)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(*(( int*)
_temp1780) == Cyc_Absyn_Comprehension_e){ _LL1960: _temp1959=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1780)->f1; goto _LL1958; _LL1958: _temp1957=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1780)->f2; goto _LL1956; _LL1956: _temp1955=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1780)->f3; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1780) == Cyc_Absyn_Struct_e){ _LL1970: _temp1969=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1780)->f1; _temp1971=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f1;
goto _LL1967; _LL1967: _temp1966=(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f2;
_temp1968=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f2; goto _LL1965;
_LL1965: _temp1964=(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f3; goto
_LL1962; _LL1962: _temp1961=(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f4;
_temp1963=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1780)->f4; goto _LL1845;}
else{ goto _LL1846;} _LL1846: if(*(( int*) _temp1780) == Cyc_Absyn_AnonStruct_e){
_LL1975: _temp1974=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1780)->f1;
goto _LL1973; _LL1973: _temp1972=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1780)->f2; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(*(( int*)
_temp1780) == Cyc_Absyn_Tunion_e){ _LL1986: _temp1985=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1780)->f1; _temp1987=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f1;
goto _LL1983; _LL1983: _temp1982=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f2;
_temp1984=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f2; goto _LL1981;
_LL1981: _temp1980=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f3; goto
_LL1979; _LL1979: _temp1978=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f4;
goto _LL1977; _LL1977: _temp1976=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1780)->f5;
goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*) _temp1780) == Cyc_Absyn_Enum_e){
_LL1993: _temp1992=(( struct Cyc_Absyn_Enum_e_struct*) _temp1780)->f1; _temp1994=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1780)->f1; goto _LL1991; _LL1991:
_temp1990=(( struct Cyc_Absyn_Enum_e_struct*) _temp1780)->f2; goto _LL1989;
_LL1989: _temp1988=(( struct Cyc_Absyn_Enum_e_struct*) _temp1780)->f3; goto
_LL1851;} else{ goto _LL1852;} _LL1852: if(*(( int*) _temp1780) == Cyc_Absyn_Malloc_e){
_LL1998: _temp1997=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1780)->f1; goto
_LL1996; _LL1996: _temp1995=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1780)->f2; goto _LL1853;} else{ goto _LL1781;} _LL1783: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1854); return; _LL1785: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1856); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1787: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1860, _temp1858); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1789: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1862); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1791: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1866, e);
goto _LL1781; _LL1793: t= Cyc_Tcexp_tcVar( te, loc, _temp1870, _temp1868); goto
_LL1781; _LL1795: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1874, _temp1872);
goto _LL1781; _LL1797: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1878,
_temp1876); goto _LL1781; _LL1799: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1884, _temp1882, _temp1880); goto _LL1781; _LL1801: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1890, _temp1888, _temp1886); goto _LL1781; _LL1803: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1894, _temp1892); goto _LL1781; _LL1805: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1901, _temp1899, _temp1898); goto _LL1781; _LL1807: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1903); goto _LL1781; _LL1809: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1907, _temp1905); goto _LL1781; _LL1811: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1911, _temp1909); goto _LL1781; _LL1813: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1913); goto _LL1781; _LL1815: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1917, e, _temp1915); goto _LL1781; _LL1817: { void* _temp1999= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1919); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1999); goto
_LL1781;} _LL1819: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1921); goto
_LL1781; _LL1821: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1925, _temp1923);
goto _LL1781; _LL1823: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1927); goto
_LL1781; _LL1825: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1931,
_temp1929); goto _LL1781; _LL1827: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1935, _temp1933); goto _LL1781; _LL1829: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1939, _temp1937); goto _LL1781; _LL1831: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1941); goto _LL1781; _LL1833: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1945, _temp1943); goto _LL1781; _LL1835: { void** elt_topt= 0;
if( topt != 0){ void* _temp2000= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp2006; void** _temp2008; _LL2002: if(( unsigned int)
_temp2000 > 4u?*(( int*) _temp2000) == Cyc_Absyn_ArrayType: 0){ _LL2007:
_temp2006=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2000)->f1;
_temp2008=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2000)->f1); goto
_LL2003;} else{ goto _LL2004;} _LL2004: goto _LL2005; _LL2003: elt_topt=( void**)
_temp2008; goto _LL2001; _LL2005: goto _LL2001; _LL2001:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1947); goto _LL1781;} _LL1837: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1949); goto _LL1781; _LL1839: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1951); goto _LL1781; _LL1841: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1953); goto _LL1781; _LL1843: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1959, _temp1957, _temp1955); goto _LL1781; _LL1845: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1971, _temp1968, _temp1964, _temp1963); goto _LL1781;
_LL1847: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1974, _temp1972); goto _LL1781;
_LL1849: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp1987, _temp1984,
_temp1980, _temp1978, _temp1976); goto _LL1781; _LL1851:* _temp1994=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp1988))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2009=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2009[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2010; _temp2010.tag= Cyc_Absyn_EnumType;
_temp2010.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1990))->name;
_temp2010.f2= _temp1990; _temp2010;}); _temp2009;}); goto _LL1781; _LL1853: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1997, _temp1995); goto _LL1781; _LL1781:;}
e->topt=({ struct Cyc_Core_Opt* _temp2011=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2011->v=( void*) t; _temp2011;});}