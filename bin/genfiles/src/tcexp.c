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
Cyc_Tcexp_make_struct_arg, es); if( _temp72->name != 0){( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp84=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp85; _temp85.tag= Cyc_Absyn_Struct_e; _temp85.f1=( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( _temp72->name))->v; _temp85.f2= 0; _temp85.f3=
_temp83; _temp85.f4=( struct Cyc_Absyn_Structdecl*) _temp72; _temp85;}); _temp84;})));}
else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name", sizeof(
unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp86=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp86[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp87; _temp87.tag= Cyc_Absyn_Struct_e;
_temp87.f1= _temp51; _temp87.f2= 0; _temp87.f3= _temp83; _temp87.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp87;}); _temp86;})));} _npop_handler( 0u); return;} _LL65: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp88= Cyc_Absynpp_qvar2string(
_temp51); xprintf("%.*s is an enum constructor, not a function", _get_arr_size(
_temp88, 1u), _temp88.curr);})); _npop_handler( 0u); return; _LL57:;};
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp90=
_temp54; _LL92: if( _temp90 == Cyc_Dict_Absent){ goto _LL93;} else{ goto _LL94;}
_LL94: goto _LL95; _LL93: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({
struct _tagged_arr _temp96= Cyc_Absynpp_qvar2string( _temp51); xprintf("unbound identifier %.*s",
_get_arr_size( _temp96, 1u), _temp96.curr);}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp97=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp98; _temp98.tag= Cyc_Absyn_FnCall_e; _temp98.f1= e1; _temp98.f2= es;
_temp98.f3= 0; _temp98;}); _temp97;}))); return; _LL95:( void) _throw( _temp90);
_LL91:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp99=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp100; _temp100.tag= Cyc_Absyn_FnCall_e;
_temp100.f1= e1; _temp100.f2= es; _temp100.f3= 0; _temp100;}); _temp99;})));
return; _LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp101=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp101[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp102; _temp102.tag= Cyc_Absyn_Array_e; _temp102.f1= des; _temp102;});
_temp101;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp103=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp113; struct Cyc_Absyn_Tqual
_temp115; void* _temp117; struct Cyc_Absyn_Structdecl** _temp119; struct Cyc_List_List*
_temp121; struct _tuple1* _temp123; struct Cyc_List_List* _temp125; _LL105: if((
unsigned int) _temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_ArrayType: 0){
_LL118: _temp117=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f3;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 > 4u?*((
int*) _temp103) == Cyc_Absyn_StructType: 0){ _LL124: _temp123=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_AnonStructType: 0){ _LL126:
_temp125=(( struct Cyc_Absyn_AnonStructType_struct*) _temp103)->f1; goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp127=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp127[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp128; _temp128.tag= Cyc_Absyn_Array_e; _temp128.f1= des; _temp128;});
_temp127;}))); goto _LL104; _LL108: if( _temp119 == 0? 1: _temp123 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("Compiler bug: struct type not properly set", sizeof(
unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp129=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp129[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp130; _temp130.tag= Cyc_Absyn_Struct_e;
_temp130.f1=( struct _tuple1*) _check_null( _temp123); _temp130.f2= 0; _temp130.f3=
des; _temp130.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp119)); _temp130;}); _temp129;}))); goto _LL104; _LL110:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp131=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp131[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp132; _temp132.tag= Cyc_Absyn_AnonStruct_e;
_temp132.f1=( void*) t; _temp132.f2= des; _temp132;}); _temp131;}))); goto
_LL104; _LL112:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp133=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp133[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp134; _temp134.tag= Cyc_Absyn_Array_e;
_temp134.f1= des; _temp134;}); _temp133;}))); goto _LL104; _LL104:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp135= msg_part; struct _tagged_arr _temp136= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_get_arr_size( _temp135, 1u), _temp135.curr, _get_arr_size( _temp136, 1u),
_temp136.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){
void* t;{ void* _temp137= c; unsigned char _temp153; void* _temp155; short
_temp157; void* _temp159; long long _temp161; void* _temp163; struct _tagged_arr
_temp165; int _temp167; void* _temp169; struct _tagged_arr _temp171; _LL139: if((
unsigned int) _temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Char_c: 0){ _LL156:
_temp155=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp137)->f1; goto _LL154;
_LL154: _temp153=(( struct Cyc_Absyn_Char_c_struct*) _temp137)->f2; goto _LL140;}
else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137)
== Cyc_Absyn_Short_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp137)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Short_c_struct*)
_temp137)->f2; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_LongLong_c: 0){ _LL164: _temp163=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp137)->f1; goto _LL162; _LL162:
_temp161=(( struct Cyc_Absyn_LongLong_c_struct*) _temp137)->f2; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137)
== Cyc_Absyn_Float_c: 0){ _LL166: _temp165=(( struct Cyc_Absyn_Float_c_struct*)
_temp137)->f1; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Int_c: 0){ _LL170: _temp169=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp137)->f1; goto _LL168; _LL168:
_temp167=(( struct Cyc_Absyn_Int_c_struct*) _temp137)->f2; goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137) ==
Cyc_Absyn_String_c: 0){ _LL172: _temp171=(( struct Cyc_Absyn_String_c_struct*)
_temp137)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if( _temp137 ==( void*)
Cyc_Absyn_Null_c){ goto _LL152;} else{ goto _LL138;} _LL140: t= _temp155 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL138;
_LL142: t= _temp159 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL138; _LL144: t= _temp163 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL138; _LL146: t= Cyc_Absyn_float_t; goto _LL138;
_LL148: if( topt == 0){ t= _temp169 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp173= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp183; void* _temp185; void* _temp187; void*
_temp189; void* _temp191; void* _temp193; _LL175: if(( unsigned int) _temp173 >
4u?*(( int*) _temp173) == Cyc_Absyn_IntType: 0){ _LL186: _temp185=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp173)->f1; goto _LL184; _LL184: _temp183=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if( _temp183 ==( void*)
Cyc_Absyn_B1){ goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177:
if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType: 0){
_LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f2; if( _temp187 ==( void*) Cyc_Absyn_B2){ goto _LL178;} else{ goto
_LL179;}} else{ goto _LL179;} _LL179: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f1; goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f2; goto _LL180;} else{ goto _LL181;} _LL181: goto _LL182; _LL176: t=
_temp185 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp195=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp195[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp196; _temp196.tag= Cyc_Absyn_Const_e;
_temp196.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp197=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp197[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp198; _temp198.tag= Cyc_Absyn_Char_c;
_temp198.f1=( void*) _temp185; _temp198.f2=( unsigned char) _temp167; _temp198;});
_temp197;})); _temp196;}); _temp195;}))); goto _LL174; _LL178: t= _temp189 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp199=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp199[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp200; _temp200.tag= Cyc_Absyn_Const_e; _temp200.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp201=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp202; _temp202.tag= Cyc_Absyn_Short_c; _temp202.f1=( void*) _temp189;
_temp202.f2=( short) _temp167; _temp202;}); _temp201;})); _temp200;}); _temp199;})));
goto _LL174; _LL180: t= _temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL174; _LL182: t= _temp169 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL174; _LL174:;} goto _LL138; _LL150: {
int len=( int) _get_arr_size( _temp171, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp203=({ struct Cyc_Absyn_Const_e_struct* _temp240=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp240[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp241; _temp241.tag= Cyc_Absyn_Const_e; _temp241.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp242=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp242[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp243; _temp243.tag= Cyc_Absyn_Int_c; _temp243.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp243.f2= len; _temp243;}); _temp242;})); _temp241;}); _temp240;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp203, loc); elen->topt=({
struct Cyc_Core_Opt* _temp204=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp204->v=( void*) Cyc_Absyn_uint_t; _temp204;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp205=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp206; _temp206.tag= Cyc_Absyn_Upper_b; _temp206.f1= elen; _temp206;});
_temp205;})); if( topt != 0){ void* _temp207= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp215; struct Cyc_Absyn_Tqual
_temp217; void* _temp219; struct Cyc_Absyn_PtrInfo _temp221; struct Cyc_Absyn_Conref*
_temp223; struct Cyc_Absyn_Tqual _temp225; struct Cyc_Absyn_Conref* _temp227;
void* _temp229; void* _temp231; _LL209: if(( unsigned int) _temp207 > 4u?*(( int*)
_temp207) == Cyc_Absyn_ArrayType: 0){ _LL220: _temp219=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f1; goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f2; goto _LL216; _LL216: _temp215=(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f3; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp207 > 4u?*(( int*) _temp207) == Cyc_Absyn_PointerType: 0){ _LL222: _temp221=((
struct Cyc_Absyn_PointerType_struct*) _temp207)->f1; _LL232: _temp231=( void*)
_temp221.elt_typ; goto _LL230; _LL230: _temp229=( void*) _temp221.rgn_typ; goto
_LL228; _LL228: _temp227= _temp221.nullable; goto _LL226; _LL226: _temp225=
_temp221.tq; goto _LL224; _LL224: _temp223= _temp221.bounds; goto _LL212;} else{
goto _LL213;} _LL213: goto _LL214; _LL210: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp233=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp234; _temp234.tag= Cyc_Absyn_ArrayType;
_temp234.f1=( void*) Cyc_Absyn_uchar_t; _temp234.f2= _temp217; _temp234.f3=(
struct Cyc_Absyn_Exp*) elen; _temp234;}); _temp233;}); _LL212: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp235=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp235->v=( void*) t; _temp235;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp236=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp236;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp237=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp237[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp238; _temp238.tag= Cyc_Absyn_Upper_b; _temp238.f1= elen; _temp238;});
_temp237;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp239=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp239->v=( void*) t; _temp239;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL208; _LL214:
goto _LL208; _LL208:;} return t;} _LL152: { struct Cyc_List_List* _temp244= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp245=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp245[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp246; _temp246.tag= Cyc_Absyn_PointerType;
_temp246.f1=({ struct Cyc_Absyn_PtrInfo _temp247; _temp247.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp249=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp249->v=( void*)
_temp244; _temp249;})); _temp247.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp248=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp248->v=( void*) _temp244;
_temp248;})); _temp247.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp247.tq= Cyc_Absyn_empty_tqual(); _temp247.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp247;}); _temp246;}); _temp245;}); goto _LL138;}
_LL138:;} return t;} static void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple1* q, void* b){ void* _temp250= b;
struct Cyc_Absyn_Vardecl* _temp264; struct Cyc_Absyn_Fndecl* _temp266; struct
Cyc_Absyn_Vardecl* _temp268; struct Cyc_Absyn_Vardecl* _temp270; struct Cyc_Absyn_Vardecl*
_temp272; _LL252: if( _temp250 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL253;}
else{ goto _LL254;} _LL254: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250)
== Cyc_Absyn_Global_b: 0){ _LL265: _temp264=(( struct Cyc_Absyn_Global_b_struct*)
_temp250)->f1; goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Funname_b: 0){ _LL267: _temp266=((
struct Cyc_Absyn_Funname_b_struct*) _temp250)->f1; goto _LL257;} else{ goto
_LL258;} _LL258: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Pat_b:
0){ _LL269: _temp268=(( struct Cyc_Absyn_Pat_b_struct*) _temp250)->f1; goto
_LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp250 > 1u?*(( int*)
_temp250) == Cyc_Absyn_Local_b: 0){ _LL271: _temp270=(( struct Cyc_Absyn_Local_b_struct*)
_temp250)->f1; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Param_b: 0){ _LL273: _temp272=((
struct Cyc_Absyn_Param_b_struct*) _temp250)->f1; goto _LL263;} else{ goto _LL251;}
_LL253: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp274= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_get_arr_size( _temp274, 1u), _temp274.curr);})); _LL255: q[
_check_known_subscript_notnull( 1u, 0)]=( _temp264->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp264->type; _LL257:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp266->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp266);
_LL259: _temp270= _temp268; goto _LL261; _LL261: _temp272= _temp270; goto _LL263;
_LL263:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp272->type; _LL251:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp275=( void*) fmt->r; void* _temp281; struct _tagged_arr
_temp283; _LL277: if(*(( int*) _temp275) == Cyc_Absyn_Const_e){ _LL282: _temp281=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp275)->f1; if(( unsigned int)
_temp281 > 1u?*(( int*) _temp281) == Cyc_Absyn_String_c: 0){ _LL284: _temp283=((
struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto _LL278;} else{ goto
_LL279;}} else{ goto _LL279;} _LL279: goto _LL280; _LL278: desc_types=
type_getter( te, _temp283, fmt->loc); goto _LL276; _LL280: Cyc_Tcutil_terr( fmt->loc,
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));
return; _LL276:;} for( 0; desc_types != 0? args != 0: 0;( desc_types=(( struct
Cyc_List_List*) _check_null( desc_types))->tl, args=(( struct Cyc_List_List*)
_check_null( args))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp285= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp286= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_get_arr_size( _temp285, 1u), _temp285.curr, _get_arr_size( _temp286, 1u),
_temp286.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments",
sizeof( unsigned char), 18u));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp287= p; _LL289: if( _temp287 ==( void*) Cyc_Absyn_Plus){ goto _LL290;}
else{ goto _LL291;} _LL291: if( _temp287 ==( void*) Cyc_Absyn_Minus){ goto
_LL292;} else{ goto _LL293;} _LL293: if( _temp287 ==( void*) Cyc_Absyn_Not){
goto _LL294;} else{ goto _LL295;} _LL295: if( _temp287 ==( void*) Cyc_Absyn_Bitnot){
goto _LL296;} else{ goto _LL297;} _LL297: if( _temp287 ==( void*) Cyc_Absyn_Size){
goto _LL298;} else{ goto _LL299;} _LL299: goto _LL300; _LL290: goto _LL292;
_LL292: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp301= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_get_arr_size( _temp301, 1u), _temp301.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL294: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp302= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_get_arr_size( _temp302, 1u), _temp302.curr);}));} return Cyc_Absyn_sint_t;
_LL296: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp303= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_get_arr_size( _temp303, 1u), _temp303.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL298:{ void* _temp304= t; struct Cyc_Absyn_Exp*
_temp312; struct Cyc_Absyn_Tqual _temp314; void* _temp316; struct Cyc_Absyn_PtrInfo
_temp318; struct Cyc_Absyn_Conref* _temp320; struct Cyc_Absyn_Tqual _temp322;
struct Cyc_Absyn_Conref* _temp324; void* _temp326; void* _temp328; _LL306: if((
unsigned int) _temp304 > 4u?*(( int*) _temp304) == Cyc_Absyn_ArrayType: 0){
_LL317: _temp316=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f1;
goto _LL315; _LL315: _temp314=(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f2;
goto _LL313; _LL313: _temp312=(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f3;
goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int) _temp304 > 4u?*((
int*) _temp304) == Cyc_Absyn_PointerType: 0){ _LL319: _temp318=(( struct Cyc_Absyn_PointerType_struct*)
_temp304)->f1; _LL329: _temp328=( void*) _temp318.elt_typ; goto _LL327; _LL327:
_temp326=( void*) _temp318.rgn_typ; goto _LL325; _LL325: _temp324= _temp318.nullable;
goto _LL323; _LL323: _temp322= _temp318.tq; goto _LL321; _LL321: _temp320=
_temp318.bounds; goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL307:
goto _LL305; _LL309: goto _LL305; _LL311: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp330= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_get_arr_size( _temp330, 1u), _temp330.curr);})); goto _LL305; _LL305:;} return
Cyc_Absyn_uint_t; _LL300: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("Non-unary primop", sizeof( unsigned char), 17u)); _LL288:;} static
void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct Cyc_Absyn_Exp*)){ if( !
checker( e1)){ Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp331= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp331, 1u), _temp331.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp332=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp332->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp332;}));}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp333= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp333, 1u), _temp333.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp334->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp334;}));}{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp335= t1; struct Cyc_Absyn_PtrInfo _temp341; struct Cyc_Absyn_Conref*
_temp343; struct Cyc_Absyn_Tqual _temp345; struct Cyc_Absyn_Conref* _temp347;
void* _temp349; void* _temp351; _LL337: if(( unsigned int) _temp335 > 4u?*(( int*)
_temp335) == Cyc_Absyn_PointerType: 0){ _LL342: _temp341=(( struct Cyc_Absyn_PointerType_struct*)
_temp335)->f1; _LL352: _temp351=( void*) _temp341.elt_typ; goto _LL350; _LL350:
_temp349=( void*) _temp341.rgn_typ; goto _LL348; _LL348: _temp347= _temp341.nullable;
goto _LL346; _LL346: _temp345= _temp341.tq; goto _LL344; _LL344: _temp343=
_temp341.bounds; goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL338:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp353= Cyc_Absynpp_typ2string( t2); xprintf("expecting int but found %.*s",
_get_arr_size( _temp353, 1u), _temp353.curr);}));} _temp343=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp343);{ void*
_temp354=( void*) _temp343->v; void* _temp362; void* _temp364; struct Cyc_Absyn_Exp*
_temp366; _LL356: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354) == Cyc_Absyn_Eq_constr:
0){ _LL363: _temp362=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp354)->f1;
if( _temp362 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL357;} else{ goto _LL358;}}
else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354)
== Cyc_Absyn_Eq_constr: 0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp354)->f1; if(( unsigned int) _temp364 > 1u?*(( int*) _temp364) == Cyc_Absyn_Upper_b:
0){ _LL367: _temp366=(( struct Cyc_Absyn_Upper_b_struct*) _temp364)->f1; goto
_LL359;} else{ goto _LL360;}} else{ goto _LL360;} _LL360: goto _LL361; _LL357:
return t1; _LL359: { struct Cyc_Absyn_PointerType_struct* _temp368=({ struct Cyc_Absyn_PointerType_struct*
_temp369=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp369[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp370; _temp370.tag= Cyc_Absyn_PointerType;
_temp370.f1=({ struct Cyc_Absyn_PtrInfo _temp371; _temp371.elt_typ=( void*)
_temp351; _temp371.rgn_typ=( void*) _temp349; _temp371.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp371.tq= _temp345; _temp371.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_Unknown_b);
_temp371;}); _temp370;}); _temp369;}); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
_temp368); return( void*) _temp368;} _LL361:( void*)( _temp343->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp372=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp372[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp373; _temp373.tag= Cyc_Absyn_Eq_constr; _temp373.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp373;}); _temp372;}))); return t1;
_LL355:;} _LL340: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL336:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp374= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr _temp375=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_get_arr_size( _temp374, 1u), _temp374.curr, _get_arr_size( _temp375, 1u),
_temp375.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp376= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp377= Cyc_Absynpp_typ2string(
t2); xprintf("expecting either %.*s or int but found %.*s", _get_arr_size(
_temp376, 1u), _temp376.curr, _get_arr_size( _temp377, 1u), _temp377.curr);}));}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp378=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp378->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp378;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp380=({ struct _tuple0 _temp379;
_temp379.f1= Cyc_Tcutil_compress( t1); _temp379.f2= Cyc_Tcutil_compress( t2);
_temp379;}); void* _temp386; struct Cyc_Absyn_PtrInfo _temp388; struct Cyc_Absyn_Conref*
_temp390; struct Cyc_Absyn_Tqual _temp392; struct Cyc_Absyn_Conref* _temp394;
void* _temp396; void* _temp398; void* _temp400; struct Cyc_Absyn_PtrInfo
_temp402; struct Cyc_Absyn_Conref* _temp404; struct Cyc_Absyn_Tqual _temp406;
struct Cyc_Absyn_Conref* _temp408; void* _temp410; void* _temp412; _LL382:
_LL401: _temp400= _temp380.f1; if(( unsigned int) _temp400 > 4u?*(( int*)
_temp400) == Cyc_Absyn_PointerType: 0){ _LL403: _temp402=(( struct Cyc_Absyn_PointerType_struct*)
_temp400)->f1; _LL413: _temp412=( void*) _temp402.elt_typ; goto _LL411; _LL411:
_temp410=( void*) _temp402.rgn_typ; goto _LL409; _LL409: _temp408= _temp402.nullable;
goto _LL407; _LL407: _temp406= _temp402.tq; goto _LL405; _LL405: _temp404=
_temp402.bounds; goto _LL387;} else{ goto _LL384;} _LL387: _temp386= _temp380.f2;
if(( unsigned int) _temp386 > 4u?*(( int*) _temp386) == Cyc_Absyn_PointerType: 0){
_LL389: _temp388=(( struct Cyc_Absyn_PointerType_struct*) _temp386)->f1; _LL399:
_temp398=( void*) _temp388.elt_typ; goto _LL397; _LL397: _temp396=( void*)
_temp388.rgn_typ; goto _LL395; _LL395: _temp394= _temp388.nullable; goto _LL393;
_LL393: _temp392= _temp388.tq; goto _LL391; _LL391: _temp390= _temp388.bounds;
goto _LL383;} else{ goto _LL384;} _LL384: goto _LL385; _LL383: if( Cyc_Tcutil_unify(
_temp412, _temp398)){ return Cyc_Absyn_sint_t;} goto _LL381; _LL385: goto _LL381;
_LL381:;} Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp414= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp415= Cyc_Absynpp_typ2string(
t2); xprintf("comparison not allowed between %.*s and %.*s", _get_arr_size(
_temp414, 1u), _temp414.curr, _get_arr_size( _temp415, 1u), _temp415.curr);}));
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp416=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp416->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp416;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp417= p; _LL419: if( _temp417 ==( void*)
Cyc_Absyn_Plus){ goto _LL420;} else{ goto _LL421;} _LL421: if( _temp417 ==( void*)
Cyc_Absyn_Minus){ goto _LL422;} else{ goto _LL423;} _LL423: if( _temp417 ==(
void*) Cyc_Absyn_Times){ goto _LL424;} else{ goto _LL425;} _LL425: if( _temp417
==( void*) Cyc_Absyn_Div){ goto _LL426;} else{ goto _LL427;} _LL427: if(
_temp417 ==( void*) Cyc_Absyn_Mod){ goto _LL428;} else{ goto _LL429;} _LL429:
if( _temp417 ==( void*) Cyc_Absyn_Bitand){ goto _LL430;} else{ goto _LL431;}
_LL431: if( _temp417 ==( void*) Cyc_Absyn_Bitor){ goto _LL432;} else{ goto
_LL433;} _LL433: if( _temp417 ==( void*) Cyc_Absyn_Bitxor){ goto _LL434;} else{
goto _LL435;} _LL435: if( _temp417 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL436;}
else{ goto _LL437;} _LL437: if( _temp417 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL438;} else{ goto _LL439;} _LL439: if( _temp417 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL440;} else{ goto _LL441;} _LL441: if( _temp417 ==( void*) Cyc_Absyn_Eq){
goto _LL442;} else{ goto _LL443;} _LL443: if( _temp417 ==( void*) Cyc_Absyn_Neq){
goto _LL444;} else{ goto _LL445;} _LL445: if( _temp417 ==( void*) Cyc_Absyn_Gt){
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp417 ==( void*) Cyc_Absyn_Lt){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp417 ==( void*) Cyc_Absyn_Gte){
goto _LL450;} else{ goto _LL451;} _LL451: if( _temp417 ==( void*) Cyc_Absyn_Lte){
goto _LL452;} else{ goto _LL453;} _LL453: goto _LL454; _LL420: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL422: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL424: goto _LL426;
_LL426: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL428: goto _LL430; _LL430: goto _LL432; _LL432: goto _LL434; _LL434: goto
_LL436; _LL436: goto _LL438; _LL438: goto _LL440; _LL440: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL442: goto _LL444; _LL444: goto _LL446;
_LL446: goto _LL448; _LL448: goto _LL450; _LL450: goto _LL452; _LL452: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL454:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp455=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp455[ 0]=({ struct Cyc_Core_Failure_struct
_temp456; _temp456.tag= Cyc_Core_Failure; _temp456.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp456;}); _temp455;})); _LL418:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp457= p; _LL459: if( _temp457
==( void*) Cyc_Absyn_Printf){ goto _LL460;} else{ goto _LL461;} _LL461: if(
_temp457 ==( void*) Cyc_Absyn_Fprintf){ goto _LL462;} else{ goto _LL463;} _LL463:
if( _temp457 ==( void*) Cyc_Absyn_Xprintf){ goto _LL464;} else{ goto _LL465;}
_LL465: if( _temp457 ==( void*) Cyc_Absyn_Scanf){ goto _LL466;} else{ goto
_LL467;} _LL467: if( _temp457 ==( void*) Cyc_Absyn_Fscanf){ goto _LL468;} else{
goto _LL469;} _LL469: if( _temp457 ==( void*) Cyc_Absyn_Sscanf){ goto _LL470;}
else{ goto _LL471;} _LL471: goto _LL472; _LL460: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("missing format string in printf", sizeof( unsigned char), 32u));}
Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd,(( struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL458; _LL462: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fprintf",
sizeof( unsigned char), 29u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp473=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp473, 1u),
_temp473.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL458;
_LL464: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in xprintf",
sizeof( unsigned char), 33u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt*
_temp474=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp474->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp474;}))); goto _LL458;
_LL466: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in scanf",
sizeof( unsigned char), 31u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL458; _LL468: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fscanf",
sizeof( unsigned char), 28u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp475=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp475, 1u),
_temp475.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL458; _LL470: Cyc_Tcutil_terr(
loc, _tag_arr("sscanf is not supported", sizeof( unsigned char), 24u)); t= Cyc_Absyn_sint_t;
goto _LL458; _LL472: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ case 0: _LL476: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL477: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL478: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL479: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL458; _LL458:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp481=( void*) e->r;
void* _temp505; struct Cyc_Absyn_Vardecl* _temp507; struct _tuple1* _temp509;
void* _temp511; struct Cyc_Absyn_Vardecl* _temp513; struct _tuple1* _temp515;
void* _temp517; struct Cyc_Absyn_Vardecl* _temp519; struct _tuple1* _temp521;
void* _temp523; struct Cyc_Absyn_Vardecl* _temp525; struct _tuple1* _temp527;
struct Cyc_Absyn_Exp* _temp529; struct Cyc_Absyn_Exp* _temp531; struct
_tagged_arr* _temp533; struct Cyc_Absyn_Exp* _temp535; struct _tagged_arr*
_temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_Absyn_Exp* _temp541; struct
Cyc_Absyn_Exp* _temp543; struct Cyc_List_List* _temp545; struct Cyc_Absyn_Exp*
_temp547; _LL483: if(*(( int*) _temp481) == Cyc_Absyn_Var_e){ _LL510: _temp509=((
struct Cyc_Absyn_Var_e_struct*) _temp481)->f1; goto _LL506; _LL506: _temp505=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f2; if(( unsigned int)
_temp505 > 1u?*(( int*) _temp505) == Cyc_Absyn_Param_b: 0){ _LL508: _temp507=((
struct Cyc_Absyn_Param_b_struct*) _temp505)->f1; goto _LL484;} else{ goto _LL485;}}
else{ goto _LL485;} _LL485: if(*(( int*) _temp481) == Cyc_Absyn_Var_e){ _LL516:
_temp515=(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f1; goto _LL512; _LL512:
_temp511=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f2; if((
unsigned int) _temp511 > 1u?*(( int*) _temp511) == Cyc_Absyn_Local_b: 0){ _LL514:
_temp513=(( struct Cyc_Absyn_Local_b_struct*) _temp511)->f1; goto _LL486;} else{
goto _LL487;}} else{ goto _LL487;} _LL487: if(*(( int*) _temp481) == Cyc_Absyn_Var_e){
_LL522: _temp521=(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f1; goto _LL518;
_LL518: _temp517=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f2; if((
unsigned int) _temp517 > 1u?*(( int*) _temp517) == Cyc_Absyn_Pat_b: 0){ _LL520:
_temp519=(( struct Cyc_Absyn_Pat_b_struct*) _temp517)->f1; goto _LL488;} else{
goto _LL489;}} else{ goto _LL489;} _LL489: if(*(( int*) _temp481) == Cyc_Absyn_Var_e){
_LL528: _temp527=(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f1; goto _LL524;
_LL524: _temp523=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp481)->f2; if((
unsigned int) _temp523 > 1u?*(( int*) _temp523) == Cyc_Absyn_Global_b: 0){
_LL526: _temp525=(( struct Cyc_Absyn_Global_b_struct*) _temp523)->f1; goto
_LL490;} else{ goto _LL491;}} else{ goto _LL491;} _LL491: if(*(( int*) _temp481)
== Cyc_Absyn_Subscript_e){ _LL532: _temp531=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp481)->f1; goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp481)->f2; goto _LL492;} else{ goto _LL493;} _LL493: if(*(( int*) _temp481)
== Cyc_Absyn_StructMember_e){ _LL536: _temp535=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp481)->f1; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp481)->f2; goto _LL494;} else{ goto _LL495;} _LL495: if(*(( int*) _temp481)
== Cyc_Absyn_StructArrow_e){ _LL540: _temp539=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp481)->f1; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp481)->f2; goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp481)
== Cyc_Absyn_Deref_e){ _LL542: _temp541=(( struct Cyc_Absyn_Deref_e_struct*)
_temp481)->f1; goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp481)
== Cyc_Absyn_NoInstantiate_e){ _LL544: _temp543=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp481)->f1; goto _LL500;} else{ goto _LL501;} _LL501: if(*(( int*) _temp481)
== Cyc_Absyn_Instantiate_e){ _LL548: _temp547=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp481)->f1; goto _LL546; _LL546: _temp545=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp481)->f2; goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL484:
_temp513= _temp507; goto _LL486; _LL486: _temp519= _temp513; goto _LL488; _LL488:
_temp525= _temp519; goto _LL490; _LL490: if( !( _temp525->tq).q_const){ return;}
goto _LL482; _LL492:{ void* _temp549= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp531->topt))->v); struct Cyc_Absyn_PtrInfo _temp559; struct Cyc_Absyn_Conref*
_temp561; struct Cyc_Absyn_Tqual _temp563; struct Cyc_Absyn_Conref* _temp565;
void* _temp567; void* _temp569; struct Cyc_Absyn_Exp* _temp571; struct Cyc_Absyn_Tqual
_temp573; void* _temp575; struct Cyc_List_List* _temp577; _LL551: if((
unsigned int) _temp549 > 4u?*(( int*) _temp549) == Cyc_Absyn_PointerType: 0){
_LL560: _temp559=(( struct Cyc_Absyn_PointerType_struct*) _temp549)->f1; _LL570:
_temp569=( void*) _temp559.elt_typ; goto _LL568; _LL568: _temp567=( void*)
_temp559.rgn_typ; goto _LL566; _LL566: _temp565= _temp559.nullable; goto _LL564;
_LL564: _temp563= _temp559.tq; goto _LL562; _LL562: _temp561= _temp559.bounds;
goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp549 > 4u?*((
int*) _temp549) == Cyc_Absyn_ArrayType: 0){ _LL576: _temp575=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp549)->f1; goto _LL574; _LL574: _temp573=((
struct Cyc_Absyn_ArrayType_struct*) _temp549)->f2; goto _LL572; _LL572: _temp571=((
struct Cyc_Absyn_ArrayType_struct*) _temp549)->f3; goto _LL554;} else{ goto
_LL555;} _LL555: if(( unsigned int) _temp549 > 4u?*(( int*) _temp549) == Cyc_Absyn_TupleType:
0){ _LL578: _temp577=(( struct Cyc_Absyn_TupleType_struct*) _temp549)->f1; goto
_LL556;} else{ goto _LL557;} _LL557: goto _LL558; _LL552: _temp573= _temp563;
goto _LL554; _LL554: if( ! _temp573.q_const){ return;} goto _LL550; _LL556: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp529);{ struct _handler_cons
_temp579; _push_handler(& _temp579);{ int _temp581= 0; if( setjmp( _temp579.handler)){
_temp581= 1;} if( ! _temp581){{ struct _tuple8 _temp584; void* _temp585; struct
Cyc_Absyn_Tqual _temp587; struct _tuple8* _temp582=(( struct _tuple8*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp577,( int) i); _temp584=* _temp582;
_LL588: _temp587= _temp584.f1; goto _LL586; _LL586: _temp585= _temp584.f2; goto
_LL583; _LL583: if( ! _temp587.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp580=( void*) _exn_thrown; void* _temp590=
_temp580; _LL592: if( _temp590 == Cyc_List_Nth){ goto _LL593;} else{ goto _LL594;}
_LL594: goto _LL595; _LL593: return; _LL595:( void) _throw( _temp590); _LL591:;}}}
goto _LL550;} _LL558: goto _LL550; _LL550:;} goto _LL482; _LL494:{ void*
_temp596= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp535->topt))->v); struct Cyc_Absyn_Structdecl** _temp608; struct Cyc_List_List*
_temp610; struct _tuple1* _temp612; struct Cyc_List_List* _temp614; struct Cyc_List_List*
_temp616; struct Cyc_Absyn_Uniondecl** _temp618; struct Cyc_List_List* _temp620;
struct _tuple1* _temp622; _LL598: if(( unsigned int) _temp596 > 4u?*(( int*)
_temp596) == Cyc_Absyn_StructType: 0){ _LL613: _temp612=(( struct Cyc_Absyn_StructType_struct*)
_temp596)->f1; goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_StructType_struct*)
_temp596)->f2; goto _LL609; _LL609: _temp608=(( struct Cyc_Absyn_StructType_struct*)
_temp596)->f3; goto _LL599;} else{ goto _LL600;} _LL600: if(( unsigned int)
_temp596 > 4u?*(( int*) _temp596) == Cyc_Absyn_AnonUnionType: 0){ _LL615:
_temp614=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp596)->f1; goto _LL601;}
else{ goto _LL602;} _LL602: if(( unsigned int) _temp596 > 4u?*(( int*) _temp596)
== Cyc_Absyn_AnonStructType: 0){ _LL617: _temp616=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp596)->f1; goto _LL603;} else{ goto _LL604;} _LL604: if(( unsigned int)
_temp596 > 4u?*(( int*) _temp596) == Cyc_Absyn_UnionType: 0){ _LL623: _temp622=((
struct Cyc_Absyn_UnionType_struct*) _temp596)->f1; goto _LL621; _LL621: _temp620=((
struct Cyc_Absyn_UnionType_struct*) _temp596)->f2; goto _LL619; _LL619: _temp618=((
struct Cyc_Absyn_UnionType_struct*) _temp596)->f3; goto _LL605;} else{ goto
_LL606;} _LL606: goto _LL607; _LL599: { struct Cyc_Absyn_Structfield* sf=
_temp608 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp608)), _temp533); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL597;} _LL601: _temp616= _temp614; goto _LL603; _LL603: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp616, _temp533); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL597;} _LL605: { struct Cyc_Absyn_Structfield*
sf= _temp618 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp618)), _temp533); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL597;} _LL607: goto _LL597; _LL597:;} goto _LL482; _LL496:{ void*
_temp624= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp539->topt))->v); struct Cyc_Absyn_PtrInfo _temp630; struct Cyc_Absyn_Conref*
_temp632; struct Cyc_Absyn_Tqual _temp634; struct Cyc_Absyn_Conref* _temp636;
void* _temp638; void* _temp640; _LL626: if(( unsigned int) _temp624 > 4u?*(( int*)
_temp624) == Cyc_Absyn_PointerType: 0){ _LL631: _temp630=(( struct Cyc_Absyn_PointerType_struct*)
_temp624)->f1; _LL641: _temp640=( void*) _temp630.elt_typ; goto _LL639; _LL639:
_temp638=( void*) _temp630.rgn_typ; goto _LL637; _LL637: _temp636= _temp630.nullable;
goto _LL635; _LL635: _temp634= _temp630.tq; goto _LL633; _LL633: _temp632=
_temp630.bounds; goto _LL627;} else{ goto _LL628;} _LL628: goto _LL629; _LL627:{
void* _temp642= Cyc_Tcutil_compress( _temp640); struct Cyc_Absyn_Structdecl**
_temp654; struct Cyc_List_List* _temp656; struct _tuple1* _temp658; struct Cyc_Absyn_Uniondecl**
_temp660; struct Cyc_List_List* _temp662; struct _tuple1* _temp664; struct Cyc_List_List*
_temp666; struct Cyc_List_List* _temp668; _LL644: if(( unsigned int) _temp642 >
4u?*(( int*) _temp642) == Cyc_Absyn_StructType: 0){ _LL659: _temp658=(( struct
Cyc_Absyn_StructType_struct*) _temp642)->f1; goto _LL657; _LL657: _temp656=((
struct Cyc_Absyn_StructType_struct*) _temp642)->f2; goto _LL655; _LL655:
_temp654=(( struct Cyc_Absyn_StructType_struct*) _temp642)->f3; goto _LL645;}
else{ goto _LL646;} _LL646: if(( unsigned int) _temp642 > 4u?*(( int*) _temp642)
== Cyc_Absyn_UnionType: 0){ _LL665: _temp664=(( struct Cyc_Absyn_UnionType_struct*)
_temp642)->f1; goto _LL663; _LL663: _temp662=(( struct Cyc_Absyn_UnionType_struct*)
_temp642)->f2; goto _LL661; _LL661: _temp660=(( struct Cyc_Absyn_UnionType_struct*)
_temp642)->f3; goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int)
_temp642 > 4u?*(( int*) _temp642) == Cyc_Absyn_AnonStructType: 0){ _LL667:
_temp666=(( struct Cyc_Absyn_AnonStructType_struct*) _temp642)->f1; goto _LL649;}
else{ goto _LL650;} _LL650: if(( unsigned int) _temp642 > 4u?*(( int*) _temp642)
== Cyc_Absyn_AnonUnionType: 0){ _LL669: _temp668=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp642)->f1; goto _LL651;} else{ goto _LL652;} _LL652: goto _LL653; _LL645: {
struct Cyc_Absyn_Structfield* sf= _temp654 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp654)), _temp537); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL643;} _LL647: { struct Cyc_Absyn_Structfield*
sf= _temp660 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp660)), _temp537); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL643;} _LL649: _temp668= _temp666; goto _LL651; _LL651: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp668, _temp537); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL643;} _LL653: goto _LL643;
_LL643:;} goto _LL625; _LL629: goto _LL625; _LL625:;} goto _LL482; _LL498:{ void*
_temp670= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp541->topt))->v); struct Cyc_Absyn_PtrInfo _temp678; struct Cyc_Absyn_Conref*
_temp680; struct Cyc_Absyn_Tqual _temp682; struct Cyc_Absyn_Conref* _temp684;
void* _temp686; void* _temp688; struct Cyc_Absyn_Exp* _temp690; struct Cyc_Absyn_Tqual
_temp692; void* _temp694; _LL672: if(( unsigned int) _temp670 > 4u?*(( int*)
_temp670) == Cyc_Absyn_PointerType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_PointerType_struct*)
_temp670)->f1; _LL689: _temp688=( void*) _temp678.elt_typ; goto _LL687; _LL687:
_temp686=( void*) _temp678.rgn_typ; goto _LL685; _LL685: _temp684= _temp678.nullable;
goto _LL683; _LL683: _temp682= _temp678.tq; goto _LL681; _LL681: _temp680=
_temp678.bounds; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp670 > 4u?*(( int*) _temp670) == Cyc_Absyn_ArrayType: 0){ _LL695: _temp694=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp670)->f1; goto _LL693; _LL693:
_temp692=(( struct Cyc_Absyn_ArrayType_struct*) _temp670)->f2; goto _LL691;
_LL691: _temp690=(( struct Cyc_Absyn_ArrayType_struct*) _temp670)->f3; goto
_LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL673: _temp692= _temp682;
goto _LL675; _LL675: if( ! _temp692.q_const){ return;} goto _LL671; _LL677: goto
_LL671; _LL671:;} goto _LL482; _LL500: _temp547= _temp543; goto _LL502; _LL502:
Cyc_Tcexp_check_writable( te, _temp547); return; _LL504: goto _LL482; _LL482:;}
Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp696= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_get_arr_size( _temp696, 1u), _temp696.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp703=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp703->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp703;})); struct Cyc_List_List* _temp697=({
struct Cyc_List_List* _temp702=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp702->hd=( void*) e3; _temp702->tl= 0; _temp702;});
struct Cyc_List_List* _temp698=({ struct Cyc_List_List* _temp701=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp701->hd=( void*) e2; _temp701->tl=
_temp697; _temp701;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp698)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp699= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); struct _tagged_arr
_temp700= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_get_arr_size( _temp699, 1u), _temp699.curr, _get_arr_size( _temp700, 1u),
_temp700.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te,
0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp704= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp710; struct Cyc_Absyn_Tqual _temp712; void* _temp714; _LL706:
if(( unsigned int) _temp704 > 4u?*(( int*) _temp704) == Cyc_Absyn_ArrayType: 0){
_LL715: _temp714=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp704)->f1;
goto _LL713; _LL713: _temp712=(( struct Cyc_Absyn_ArrayType_struct*) _temp704)->f2;
goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_ArrayType_struct*) _temp704)->f3;
goto _LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL707: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL705; _LL709: goto _LL705; _LL705:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp716= Cyc_Absynpp_typ2string( t1);
struct _tagged_arr _temp717= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_get_arr_size( _temp716, 1u), _temp716.curr, _get_arr_size( _temp717, 1u),
_temp717.curr);}));}} else{ void* _temp718=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; void* _temp719= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
_temp718, e1, e2); if( !( Cyc_Tcutil_unify( _temp719, t1)? 1: Cyc_Tcutil_coerceable(
_temp719))){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment",
sizeof( unsigned char), 42u));} return _temp719;} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; for( fields= fs; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp722;
struct Cyc_Position_Segment* _temp724; struct Cyc_List_List* _temp726; struct
Cyc_List_List* _temp728; struct _tuple1* _temp730; struct Cyc_Absyn_Tunionfield
_temp720=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL731: _temp730= _temp720.name; goto _LL729; _LL729: _temp728=
_temp720.tvs; goto _LL727; _LL727: _temp726= _temp720.typs; goto _LL725; _LL725:
_temp724= _temp720.loc; goto _LL723; _LL723: _temp722=( void*) _temp720.sc; goto
_LL721; _LL721: if(( _temp726 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp726))->tl != 0)? 1: _temp728 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp726))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp734; struct Cyc_Position_Segment* _temp736; struct Cyc_List_List*
_temp738; struct Cyc_List_List* _temp740; struct _tuple1* _temp742; struct Cyc_Absyn_Tunionfield
_temp732=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL743: _temp742= _temp732.name; goto _LL741; _LL741: _temp740=
_temp732.tvs; goto _LL739; _LL739: _temp738= _temp732.typs; goto _LL737; _LL737:
_temp736= _temp732.loc; goto _LL735; _LL735: _temp734=( void*) _temp732.sc; goto
_LL733; _LL733: if(( _temp738 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp738))->tl != 0)? 1: _temp740 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp738))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp744= Cyc_Absynpp_typ2string(
tu); struct _tagged_arr _temp745= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_get_arr_size( _temp744, 1u), _temp744.curr, _get_arr_size( _temp745, 1u),
_temp745.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){
te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp746= t;
struct Cyc_Absyn_PtrInfo _temp752; struct Cyc_Absyn_Conref* _temp754; struct Cyc_Absyn_Tqual
_temp756; struct Cyc_Absyn_Conref* _temp758; void* _temp760; void* _temp762;
_LL748: if(( unsigned int) _temp746 > 4u?*(( int*) _temp746) == Cyc_Absyn_PointerType:
0){ _LL753: _temp752=(( struct Cyc_Absyn_PointerType_struct*) _temp746)->f1;
_LL763: _temp762=( void*) _temp752.elt_typ; goto _LL761; _LL761: _temp760=( void*)
_temp752.rgn_typ; goto _LL759; _LL759: _temp758= _temp752.nullable; goto _LL757;
_LL757: _temp756= _temp752.tq; goto _LL755; _LL755: _temp754= _temp752.bounds;
goto _LL749;} else{ goto _LL750;} _LL750: goto _LL751; _LL749: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp760); Cyc_Tcutil_check_nonzero_bound( loc, _temp754);{ void*
_temp764= Cyc_Tcutil_compress( _temp762); struct Cyc_Absyn_FnInfo _temp770;
struct Cyc_List_List* _temp772; struct Cyc_List_List* _temp774; struct Cyc_Absyn_VarargInfo*
_temp776; int _temp778; struct Cyc_List_List* _temp780; void* _temp782; struct
Cyc_Core_Opt* _temp784; struct Cyc_List_List* _temp786; _LL766: if((
unsigned int) _temp764 > 4u?*(( int*) _temp764) == Cyc_Absyn_FnType: 0){ _LL771:
_temp770=(( struct Cyc_Absyn_FnType_struct*) _temp764)->f1; _LL787: _temp786=
_temp770.tvars; goto _LL785; _LL785: _temp784= _temp770.effect; goto _LL783;
_LL783: _temp782=( void*) _temp770.ret_typ; goto _LL781; _LL781: _temp780=
_temp770.args; goto _LL779; _LL779: _temp778= _temp770.c_varargs; goto _LL777;
_LL777: _temp776= _temp770.cyc_varargs; goto _LL775; _LL775: _temp774= _temp770.rgn_po;
goto _LL773; _LL773: _temp772= _temp770.attributes; goto _LL767;} else{ goto
_LL768;} _LL768: goto _LL769; _LL767: if( topt != 0){ Cyc_Tcutil_unify( _temp782,*((
void**) _check_null( topt)));} while( es != 0? _temp780 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp780))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp788= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr
_temp789= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_get_arr_size( _temp788, 1u), _temp788.curr, _get_arr_size( _temp789, 1u),
_temp789.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp780=(( struct Cyc_List_List*) _check_null( _temp780))->tl;} if( _temp780 !=
0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function", sizeof(
unsigned char), 31u));} else{ if( es != 0){ if( _temp778){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}} else{ if(
_temp776 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function",
sizeof( unsigned char), 32u));} else{ int _temp792; void* _temp794; void*
_temp796; struct Cyc_Absyn_Tqual _temp798; struct Cyc_Core_Opt* _temp800; struct
Cyc_Absyn_VarargInfo _temp790=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp776)); _LL801: _temp800= _temp790.name; goto _LL799; _LL799: _temp798=
_temp790.tq; goto _LL797; _LL797: _temp796=( void*) _temp790.type; goto _LL795;
_LL795: _temp794=( void*) _temp790.rgn; goto _LL793; _LL793: _temp792= _temp790.inject;
goto _LL791; _LL791: { struct Cyc_Absyn_VarargCallInfo* _temp802=({ struct Cyc_Absyn_VarargCallInfo*
_temp829=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp829->num_varargs= 0; _temp829->injectors= 0; _temp829->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp776); _temp829;});* vararg_call_info= _temp802; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp794)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp803= Cyc_Absynpp_typ2string( _temp794); struct
_tagged_arr _temp804= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te)); xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_get_arr_size( _temp803, 1u), _temp803.curr, _get_arr_size( _temp804, 1u),
_temp804.curr);}));} if( ! _temp792){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp802))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp796,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp796)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp805= Cyc_Absynpp_typ2string(
_temp796); struct _tagged_arr _temp806= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_get_arr_size( _temp805, 1u), _temp805.curr, _get_arr_size( _temp806, 1u),
_temp806.curr);}));}}} else{ void* _temp807= Cyc_Tcutil_compress( _temp796);
struct Cyc_Absyn_TunionInfo _temp813; void* _temp815; struct Cyc_List_List*
_temp817; void* _temp819; struct Cyc_Absyn_Tuniondecl* _temp821; _LL809: if((
unsigned int) _temp807 > 4u?*(( int*) _temp807) == Cyc_Absyn_TunionType: 0){
_LL814: _temp813=(( struct Cyc_Absyn_TunionType_struct*) _temp807)->f1; _LL820:
_temp819=( void*) _temp813.tunion_info; if(*(( int*) _temp819) == Cyc_Absyn_KnownTunion){
_LL822: _temp821=(( struct Cyc_Absyn_KnownTunion_struct*) _temp819)->f1; goto
_LL818;} else{ goto _LL811;} _LL818: _temp817= _temp813.targs; goto _LL816;
_LL816: _temp815=( void*) _temp813.rgn; goto _LL810;} else{ goto _LL811;} _LL811:
goto _LL812; _LL810: { struct Cyc_Absyn_Tuniondecl* _temp823=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp821->name); if( ! Cyc_Tcutil_unify( _temp815, _temp794)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("bad region for injected varargs"));}{ struct
Cyc_List_List* fields= 0; if( _temp823->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp824= Cyc_Absynpp_typ2string(
_temp796); xprintf("can't inject into %.*s", _get_arr_size( _temp824, 1u),
_temp824.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp823->fields))->v;}{ struct _RegionHandle _temp825= _new_region();
struct _RegionHandle* rgn=& _temp825; _push_region( rgn);{ struct Cyc_List_List*
_temp826=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp823->tvs, _temp817); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*) _check_null(
_temp802))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp827= Cyc_Tcexp_tcInjection( te, e1, _temp796,
rgn, _temp826, fields); if( _temp827 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp802))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp802))->injectors,({ struct Cyc_List_List* _temp828=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp828->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp827)); _temp828->tl= 0;
_temp828;}));}}}}}; _pop_region( rgn);} goto _LL808;}} _LL812: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL808; _LL808:;}}}}}} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp784))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp774); return _temp782; _LL769: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL765:;}
_LL751: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL747:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp830= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_get_arr_size( _temp830, 1u), _temp830.curr);}));} return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp831=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp831->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp831;}));}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp832= t1; struct Cyc_Absyn_PtrInfo
_temp838; struct Cyc_Absyn_Conref* _temp840; struct Cyc_Absyn_Tqual _temp842;
struct Cyc_Absyn_Conref* _temp844; void* _temp846; void* _temp848; _LL834: if((
unsigned int) _temp832 > 4u?*(( int*) _temp832) == Cyc_Absyn_PointerType: 0){
_LL839: _temp838=(( struct Cyc_Absyn_PointerType_struct*) _temp832)->f1; _LL849:
_temp848=( void*) _temp838.elt_typ; goto _LL847; _LL847: _temp846=( void*)
_temp838.rgn_typ; goto _LL845; _LL845: _temp844= _temp838.nullable; goto _LL843;
_LL843: _temp842= _temp838.tq; goto _LL841; _LL841: _temp840= _temp838.bounds;
goto _LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835:{ void* _temp850=
Cyc_Tcutil_compress( _temp848); struct Cyc_Absyn_FnInfo _temp856; struct Cyc_List_List*
_temp858; struct Cyc_List_List* _temp860; struct Cyc_Absyn_VarargInfo* _temp862;
int _temp864; struct Cyc_List_List* _temp866; void* _temp868; struct Cyc_Core_Opt*
_temp870; struct Cyc_List_List* _temp872; _LL852: if(( unsigned int) _temp850 >
4u?*(( int*) _temp850) == Cyc_Absyn_FnType: 0){ _LL857: _temp856=(( struct Cyc_Absyn_FnType_struct*)
_temp850)->f1; _LL873: _temp872= _temp856.tvars; goto _LL871; _LL871: _temp870=
_temp856.effect; goto _LL869; _LL869: _temp868=( void*) _temp856.ret_typ; goto
_LL867; _LL867: _temp866= _temp856.args; goto _LL865; _LL865: _temp864= _temp856.c_varargs;
goto _LL863; _LL863: _temp862= _temp856.cyc_varargs; goto _LL861; _LL861:
_temp860= _temp856.rgn_po; goto _LL859; _LL859: _temp858= _temp856.attributes;
goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855; _LL853: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp872)){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u));}
for( 0; ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, _temp872=((
struct Cyc_List_List*) _check_null( _temp872))->tl)){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp872))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp874=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp874->hd=( void*)({
struct _tuple5* _temp875=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp875->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp872))->hd; _temp875->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp875;}); _temp874->tl= instantiation; _temp874;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp879=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp879[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp880; _temp880.tag= Cyc_Absyn_FnType; _temp880.f1=({ struct Cyc_Absyn_FnInfo
_temp881; _temp881.tvars= 0; _temp881.effect= _temp870; _temp881.ret_typ=( void*)
_temp868; _temp881.args= _temp866; _temp881.c_varargs= _temp864; _temp881.cyc_varargs=
_temp862; _temp881.rgn_po= _temp860; _temp881.attributes= _temp858; _temp881;});
_temp880;}); _temp879;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp876=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp876[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp877; _temp877.tag= Cyc_Absyn_PointerType;
_temp877.f1=({ struct Cyc_Absyn_PtrInfo _temp878; _temp878.elt_typ=( void*)
new_fn_typ; _temp878.rgn_typ=( void*) _temp846; _temp878.nullable= _temp844;
_temp878.tq= _temp842; _temp878.bounds= _temp840; _temp878;}); _temp877;});
_temp876;}); return new_typ;}} _LL855: goto _LL851; _LL851:;} goto _LL833;
_LL837: goto _LL833; _LL833:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp882= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_get_arr_size( _temp882, 1u), _temp882.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp883= Cyc_Absynpp_typ2string(
t2); struct _tagged_arr _temp884= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_get_arr_size( _temp883, 1u), _temp883.curr, _get_arr_size( _temp884, 1u),
_temp884.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp885= 0; struct Cyc_Absyn_Tqual _temp886= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp887= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp893; struct Cyc_Absyn_Conref* _temp895;
struct Cyc_Absyn_Tqual _temp897; struct Cyc_Absyn_Conref* _temp899; void*
_temp901; void* _temp903; _LL889: if(( unsigned int) _temp887 > 4u?*(( int*)
_temp887) == Cyc_Absyn_PointerType: 0){ _LL894: _temp893=(( struct Cyc_Absyn_PointerType_struct*)
_temp887)->f1; _LL904: _temp903=( void*) _temp893.elt_typ; goto _LL902; _LL902:
_temp901=( void*) _temp893.rgn_typ; goto _LL900; _LL900: _temp899= _temp893.nullable;
goto _LL898; _LL898: _temp897= _temp893.tq; goto _LL896; _LL896: _temp895=
_temp893.bounds; goto _LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL890:
_temp885=({ void** _temp905=( void**) GC_malloc( sizeof( void*)); _temp905[ 0]=
_temp903; _temp905;}); _temp886= _temp897; goto _LL888; _LL892: goto _LL888;
_LL888:;} Cyc_Tcexp_tcExpNoInst( te, _temp885, e);{ void* _temp906=( void*) e->r;
struct Cyc_Absyn_Structdecl* _temp914; struct Cyc_List_List* _temp916; struct
Cyc_Core_Opt* _temp918; struct _tuple1* _temp920; struct Cyc_List_List* _temp922;
_LL908: if(*(( int*) _temp906) == Cyc_Absyn_Struct_e){ _LL921: _temp920=((
struct Cyc_Absyn_Struct_e_struct*) _temp906)->f1; goto _LL919; _LL919: _temp918=((
struct Cyc_Absyn_Struct_e_struct*) _temp906)->f2; goto _LL917; _LL917: _temp916=((
struct Cyc_Absyn_Struct_e_struct*) _temp906)->f3; goto _LL915; _LL915: _temp914=((
struct Cyc_Absyn_Struct_e_struct*) _temp906)->f4; goto _LL909;} else{ goto
_LL910;} _LL910: if(*(( int*) _temp906) == Cyc_Absyn_Tuple_e){ _LL923: _temp922=((
struct Cyc_Absyn_Tuple_e_struct*) _temp906)->f1; goto _LL911;} else{ goto _LL912;}
_LL912: goto _LL913; _LL909: goto _LL911; _LL911: Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u));{ void*(* _temp924)( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp925= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp931; struct Cyc_Absyn_Conref*
_temp933; struct Cyc_Absyn_Tqual _temp935; struct Cyc_Absyn_Conref* _temp937;
void* _temp939; void* _temp941; _LL927: if(( unsigned int) _temp925 > 4u?*(( int*)
_temp925) == Cyc_Absyn_PointerType: 0){ _LL932: _temp931=(( struct Cyc_Absyn_PointerType_struct*)
_temp925)->f1; _LL942: _temp941=( void*) _temp931.elt_typ; goto _LL940; _LL940:
_temp939=( void*) _temp931.rgn_typ; goto _LL938; _LL938: _temp937= _temp931.nullable;
goto _LL936; _LL936: _temp935= _temp931.tq; goto _LL934; _LL934: _temp933=
_temp931.bounds; goto _LL928;} else{ goto _LL929;} _LL929: goto _LL930; _LL928:
if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp937)){
_temp924= Cyc_Absyn_star_typ;} goto _LL926; _LL930: goto _LL926; _LL926:;}
return _temp924(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp886);} _LL913: goto _LL907; _LL907:;}{ void*
_temp945; int _temp947; struct _tuple6 _temp943= Cyc_Tcutil_addressof_props( te,
e); _LL948: _temp947= _temp943.f1; goto _LL946; _LL946: _temp945= _temp943.f2;
goto _LL944; _LL944: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp947){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp945, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp949= Cyc_Absynpp_typ2string( t); xprintf("sizeof applied to type %.*s, which has unknown size here",
_get_arr_size( _temp949, 1u), _temp949.curr);}));} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp950= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp958; struct Cyc_List_List* _temp960; struct _tuple1* _temp962; struct Cyc_List_List*
_temp964; _LL952: if(( unsigned int) _temp950 > 4u?*(( int*) _temp950) == Cyc_Absyn_StructType:
0){ _LL963: _temp962=(( struct Cyc_Absyn_StructType_struct*) _temp950)->f1; goto
_LL961; _LL961: _temp960=(( struct Cyc_Absyn_StructType_struct*) _temp950)->f2;
goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_StructType_struct*) _temp950)->f3;
goto _LL953;} else{ goto _LL954;} _LL954: if(( unsigned int) _temp950 > 4u?*((
int*) _temp950) == Cyc_Absyn_AnonStructType: 0){ _LL965: _temp964=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp950)->f1; goto _LL955;} else{ goto _LL956;} _LL956: goto _LL957; _LL953:
if( _temp958 == 0){ return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u));} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp958)))->fields == 0){ goto
_LL951;} goto _LL955; _LL955: bad_type= 0; goto _LL951; _LL957: goto _LL951;
_LL951:;} if( bad_type){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp966= Cyc_Absynpp_typ2string( t); xprintf("%.*s is not a known struct type",
_get_arr_size( _temp966, 1u), _temp966.curr);}));} return Cyc_Absyn_uint_t;}}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp967= t; struct Cyc_Absyn_PtrInfo _temp973; struct Cyc_Absyn_Conref*
_temp975; struct Cyc_Absyn_Tqual _temp977; struct Cyc_Absyn_Conref* _temp979;
void* _temp981; void* _temp983; _LL969: if(( unsigned int) _temp967 > 4u?*(( int*)
_temp967) == Cyc_Absyn_PointerType: 0){ _LL974: _temp973=(( struct Cyc_Absyn_PointerType_struct*)
_temp967)->f1; _LL984: _temp983=( void*) _temp973.elt_typ; goto _LL982; _LL982:
_temp981=( void*) _temp973.rgn_typ; goto _LL980; _LL980: _temp979= _temp973.nullable;
goto _LL978; _LL978: _temp977= _temp973.tq; goto _LL976; _LL976: _temp975=
_temp973.bounds; goto _LL970;} else{ goto _LL971;} _LL971: goto _LL972; _LL970:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp981); Cyc_Tcutil_check_nonzero_bound(
loc, _temp975); return _temp983; _LL972: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp985= Cyc_Absynpp_typ2string( t);
xprintf("expecting * or @ type but found %.*s", _get_arr_size( _temp985, 1u),
_temp985.curr);})); _LL968:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp986= t; struct Cyc_Absyn_Structdecl** _temp1002;
struct Cyc_Absyn_Structdecl* _temp1004; struct Cyc_List_List* _temp1005; struct
_tuple1* _temp1007; struct Cyc_List_List* _temp1009; struct Cyc_List_List*
_temp1011; struct Cyc_Absyn_Uniondecl** _temp1013; struct Cyc_Absyn_Uniondecl*
_temp1015; struct Cyc_List_List* _temp1016; struct _tuple1* _temp1018; struct
Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Tqual _temp1023; void* _temp1025;
struct Cyc_Absyn_PtrInfo _temp1028; _LL988: if(( unsigned int) _temp986 > 4u?*((
int*) _temp986) == Cyc_Absyn_StructType: 0){ _LL1008: _temp1007=(( struct Cyc_Absyn_StructType_struct*)
_temp986)->f1; goto _LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_StructType_struct*)
_temp986)->f2; goto _LL1003; _LL1003: _temp1002=(( struct Cyc_Absyn_StructType_struct*)
_temp986)->f3; if( _temp1002 == 0){ goto _LL990;} else{ _temp1004=* _temp1002;
goto _LL989;}} else{ goto _LL990;} _LL990: if(( unsigned int) _temp986 > 4u?*((
int*) _temp986) == Cyc_Absyn_AnonStructType: 0){ _LL1010: _temp1009=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp986)->f1; goto _LL991;} else{ goto _LL992;}
_LL992: if(( unsigned int) _temp986 > 4u?*(( int*) _temp986) == Cyc_Absyn_AnonUnionType:
0){ _LL1012: _temp1011=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp986)->f1;
goto _LL993;} else{ goto _LL994;} _LL994: if(( unsigned int) _temp986 > 4u?*((
int*) _temp986) == Cyc_Absyn_UnionType: 0){ _LL1019: _temp1018=(( struct Cyc_Absyn_UnionType_struct*)
_temp986)->f1; goto _LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_UnionType_struct*)
_temp986)->f2; goto _LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_UnionType_struct*)
_temp986)->f3; if( _temp1013 == 0){ goto _LL996;} else{ _temp1015=* _temp1013;
goto _LL995;}} else{ goto _LL996;} _LL996: if(( unsigned int) _temp986 > 4u?*((
int*) _temp986) == Cyc_Absyn_ArrayType: 0){ _LL1026: _temp1025=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp986)->f1; goto _LL1024; _LL1024: _temp1023=((
struct Cyc_Absyn_ArrayType_struct*) _temp986)->f2; goto _LL1022; _LL1022:
_temp1021=(( struct Cyc_Absyn_ArrayType_struct*) _temp986)->f3; goto _LL1020;}
else{ goto _LL998;} _LL1020: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ goto _LL997;} else{ goto _LL998;} _LL998:
if(( unsigned int) _temp986 > 4u?*(( int*) _temp986) == Cyc_Absyn_PointerType: 0){
_LL1029: _temp1028=(( struct Cyc_Absyn_PointerType_struct*) _temp986)->f1; goto
_LL1027;} else{ goto _LL1000;} _LL1027: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ goto _LL999;} else{ goto _LL1000;} _LL1000:
goto _LL1001; _LL989: if( _temp1007 == 0){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("unresolved struct type (COMPILER ERROR)", sizeof( unsigned char), 40u));}{
struct Cyc_Absyn_Structfield* _temp1030= Cyc_Absyn_lookup_struct_field(
_temp1004, f); if( _temp1030 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1031= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1007)); struct _tagged_arr _temp1032=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp1031, 1u), _temp1031.curr, _get_arr_size( _temp1032, 1u),
_temp1032.curr);}));}{ void* t2;{ struct _RegionHandle _temp1033= _new_region();
struct _RegionHandle* rgn=& _temp1033; _push_region( rgn);{ struct Cyc_List_List*
_temp1034=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1004->tvs, _temp1005); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1034,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1030))->type);};
_pop_region( rgn);} return t2;}} _LL991: { struct Cyc_Absyn_Structfield*
_temp1035= Cyc_Absyn_lookup_field( _temp1009, f); if( _temp1035 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1036=*
f; xprintf("struct has no %.*s field", _get_arr_size( _temp1036, 1u), _temp1036.curr);}));}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1035))->type;}
_LL993: { struct Cyc_Absyn_Structfield* _temp1037= Cyc_Absyn_lookup_field(
_temp1011, f); if( _temp1037 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1038=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1038, 1u), _temp1038.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1037))->type;} _LL995: if( _temp1018 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp1039= Cyc_Absyn_lookup_union_field(
_temp1015, f); if( _temp1039 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1040= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1018)); struct _tagged_arr _temp1041=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp1040, 1u), _temp1040.curr, _get_arr_size( _temp1041, 1u),
_temp1041.curr);}));}{ struct Cyc_List_List* _temp1042=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1015->tvs,
_temp1016); void* _temp1043= Cyc_Tcutil_substitute( _temp1042,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1039))->type); return _temp1043;}}
_LL997:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1044=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1044[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1045; _temp1045.tag= Cyc_Absyn_Primop_e;
_temp1045.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1045.f2=({ struct Cyc_List_List*
_temp1046=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1046->hd=( void*) e; _temp1046->tl= 0; _temp1046;}); _temp1045;});
_temp1044;}))); return Cyc_Absyn_uint_t; _LL999:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1047=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1047[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1048; _temp1048.tag= Cyc_Absyn_Primop_e;
_temp1048.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1048.f2=({ struct Cyc_List_List*
_temp1049=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1049->hd=( void*) e; _temp1049->tl= 0; _temp1049;}); _temp1048;});
_temp1047;}))); return Cyc_Absyn_uint_t; _LL1001: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1050= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct, union or array, found %.*s", _get_arr_size(
_temp1050, 1u), _temp1050.curr);}));} else{ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1051= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct or union, found %.*s", _get_arr_size( _temp1051, 1u),
_temp1051.curr);}));} _LL987:;}} static void* Cyc_Tcexp_tcStructArrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e, struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1052= t;
struct Cyc_Absyn_PtrInfo _temp1058; struct Cyc_Absyn_Conref* _temp1060; struct
Cyc_Absyn_Tqual _temp1062; struct Cyc_Absyn_Conref* _temp1064; void* _temp1066;
void* _temp1068; _LL1054: if(( unsigned int) _temp1052 > 4u?*(( int*) _temp1052)
== Cyc_Absyn_PointerType: 0){ _LL1059: _temp1058=(( struct Cyc_Absyn_PointerType_struct*)
_temp1052)->f1; _LL1069: _temp1068=( void*) _temp1058.elt_typ; goto _LL1067;
_LL1067: _temp1066=( void*) _temp1058.rgn_typ; goto _LL1065; _LL1065: _temp1064=
_temp1058.nullable; goto _LL1063; _LL1063: _temp1062= _temp1058.tq; goto _LL1061;
_LL1061: _temp1060= _temp1058.bounds; goto _LL1055;} else{ goto _LL1056;}
_LL1056: goto _LL1057; _LL1055: Cyc_Tcutil_check_nonzero_bound( loc, _temp1060);{
void* _temp1070= Cyc_Tcutil_compress( _temp1068); struct Cyc_Absyn_Structdecl**
_temp1082; struct Cyc_Absyn_Structdecl* _temp1084; struct Cyc_List_List*
_temp1085; struct _tuple1* _temp1087; struct Cyc_Absyn_Uniondecl** _temp1089;
struct Cyc_Absyn_Uniondecl* _temp1091; struct Cyc_List_List* _temp1092; struct
_tuple1* _temp1094; struct Cyc_List_List* _temp1096; struct Cyc_List_List*
_temp1098; _LL1072: if(( unsigned int) _temp1070 > 4u?*(( int*) _temp1070) ==
Cyc_Absyn_StructType: 0){ _LL1088: _temp1087=(( struct Cyc_Absyn_StructType_struct*)
_temp1070)->f1; goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_StructType_struct*)
_temp1070)->f2; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_StructType_struct*)
_temp1070)->f3; if( _temp1082 == 0){ goto _LL1074;} else{ _temp1084=* _temp1082;
goto _LL1073;}} else{ goto _LL1074;} _LL1074: if(( unsigned int) _temp1070 > 4u?*((
int*) _temp1070) == Cyc_Absyn_UnionType: 0){ _LL1095: _temp1094=(( struct Cyc_Absyn_UnionType_struct*)
_temp1070)->f1; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_UnionType_struct*)
_temp1070)->f2; goto _LL1090; _LL1090: _temp1089=(( struct Cyc_Absyn_UnionType_struct*)
_temp1070)->f3; if( _temp1089 == 0){ goto _LL1076;} else{ _temp1091=* _temp1089;
goto _LL1075;}} else{ goto _LL1076;} _LL1076: if(( unsigned int) _temp1070 > 4u?*((
int*) _temp1070) == Cyc_Absyn_AnonStructType: 0){ _LL1097: _temp1096=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1070)->f1; goto _LL1077;} else{ goto
_LL1078;} _LL1078: if(( unsigned int) _temp1070 > 4u?*(( int*) _temp1070) == Cyc_Absyn_AnonUnionType:
0){ _LL1099: _temp1098=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1070)->f1;
goto _LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081; _LL1073: { struct Cyc_Absyn_Structfield*
_temp1100= Cyc_Absyn_lookup_struct_field( _temp1084, f); if( _temp1100 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1101= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1087));
struct _tagged_arr _temp1102=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp1101, 1u), _temp1101.curr, _get_arr_size( _temp1102, 1u),
_temp1102.curr);}));}{ void* t3;{ struct _RegionHandle _temp1103= _new_region();
struct _RegionHandle* rgn=& _temp1103; _push_region( rgn);{ struct Cyc_List_List*
_temp1104=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1084->tvs, _temp1085); t3= Cyc_Tcutil_rsubstitute( rgn, _temp1104,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1100))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1066); return
t3;}} _LL1075: { struct Cyc_Absyn_Structfield* _temp1105= Cyc_Absyn_lookup_union_field(
_temp1091, f); if( _temp1105 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1106= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1094)); struct _tagged_arr _temp1107=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp1106, 1u), _temp1106.curr, _get_arr_size( _temp1107, 1u),
_temp1107.curr);}));}{ struct Cyc_List_List* _temp1108=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1091->tvs,
_temp1092); void* _temp1109= Cyc_Tcutil_substitute( _temp1108,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1105))->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1066); return _temp1109;}} _LL1077: { struct Cyc_Absyn_Structfield*
_temp1110= Cyc_Absyn_lookup_field( _temp1096, f); if( _temp1110 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1111=*
f; xprintf("struct has no %.*s field", _get_arr_size( _temp1111, 1u), _temp1111.curr);}));}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1110))->type;}
_LL1079: { struct Cyc_Absyn_Structfield* _temp1112= Cyc_Absyn_lookup_field(
_temp1098, f); if( _temp1112 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1113=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1113, 1u), _temp1113.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1112))->type;} _LL1081: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1114= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1114, 1u),
_temp1114.curr);})); _LL1071:;} _LL1057: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1115= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1115, 1u),
_temp1115.curr);})); _LL1053:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1116; _push_handler(& _temp1116);{ int _temp1118= 0; if(
setjmp( _temp1116.handler)){ _temp1118= 1;} if( ! _temp1118){{ void* _temp1119=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1119;}; _pop_handler();} else{ void* _temp1117=(
void*) _exn_thrown; void* _temp1121= _temp1117; _LL1123: if( _temp1121 == Cyc_List_Nth){
goto _LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126; _LL1124: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr) xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1126:(
void) _throw( _temp1121); _LL1122:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1127= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_get_arr_size( _temp1127, 1u), _temp1127.curr);}));}{ void* _temp1128= t1;
struct Cyc_Absyn_PtrInfo _temp1136; struct Cyc_Absyn_Conref* _temp1138; struct
Cyc_Absyn_Tqual _temp1140; struct Cyc_Absyn_Conref* _temp1142; void* _temp1144;
void* _temp1146; struct Cyc_List_List* _temp1148; _LL1130: if(( unsigned int)
_temp1128 > 4u?*(( int*) _temp1128) == Cyc_Absyn_PointerType: 0){ _LL1137:
_temp1136=(( struct Cyc_Absyn_PointerType_struct*) _temp1128)->f1; _LL1147:
_temp1146=( void*) _temp1136.elt_typ; goto _LL1145; _LL1145: _temp1144=( void*)
_temp1136.rgn_typ; goto _LL1143; _LL1143: _temp1142= _temp1136.nullable; goto
_LL1141; _LL1141: _temp1140= _temp1136.tq; goto _LL1139; _LL1139: _temp1138=
_temp1136.bounds; goto _LL1131;} else{ goto _LL1132;} _LL1132: if(( unsigned int)
_temp1128 > 4u?*(( int*) _temp1128) == Cyc_Absyn_TupleType: 0){ _LL1149:
_temp1148=(( struct Cyc_Absyn_TupleType_struct*) _temp1128)->f1; goto _LL1133;}
else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1131: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1138);} else{{ void* _temp1150=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1138))->v; void*
_temp1156; struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp _temp1160;
struct Cyc_Position_Segment* _temp1161; void* _temp1163; void* _temp1165; int
_temp1167; void* _temp1169; struct Cyc_Core_Opt* _temp1171; _LL1152: if((
unsigned int) _temp1150 > 1u?*(( int*) _temp1150) == Cyc_Absyn_Eq_constr: 0){
_LL1157: _temp1156=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1150)->f1;
if(( unsigned int) _temp1156 > 1u?*(( int*) _temp1156) == Cyc_Absyn_Upper_b: 0){
_LL1159: _temp1158=(( struct Cyc_Absyn_Upper_b_struct*) _temp1156)->f1;
_temp1160=* _temp1158; _LL1172: _temp1171= _temp1160.topt; goto _LL1164; _LL1164:
_temp1163=( void*) _temp1160.r; if(*(( int*) _temp1163) == Cyc_Absyn_Const_e){
_LL1166: _temp1165=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1163)->f1;
if(( unsigned int) _temp1165 > 1u?*(( int*) _temp1165) == Cyc_Absyn_Int_c: 0){
_LL1170: _temp1169=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1165)->f1;
if( _temp1169 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1168;} else{ goto _LL1154;}
_LL1168: _temp1167=(( struct Cyc_Absyn_Int_c_struct*) _temp1165)->f2; if(
_temp1167 == 1){ goto _LL1162;} else{ goto _LL1154;}} else{ goto _LL1154;}}
else{ goto _LL1154;} _LL1162: _temp1161= _temp1160.loc; goto _LL1153;} else{
goto _LL1154;}} else{ goto _LL1154;} _LL1154: goto _LL1155; _LL1153: Cyc_Tcutil_warn(
e1->loc, _tag_arr("subscript applied to pointer to one object", sizeof(
unsigned char), 43u)); goto _LL1151; _LL1155: goto _LL1151; _LL1151:;} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1138);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1144); return
_temp1146; _LL1133: return Cyc_Tcexp_ithTupleType( te, loc, _temp1148, e2);
_LL1135: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1173= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_get_arr_size( _temp1173, 1u), _temp1173.curr);})); _LL1129:;}}} static void*
Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1174= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1180; _LL1176: if((
unsigned int) _temp1174 > 4u?*(( int*) _temp1174) == Cyc_Absyn_TupleType: 0){
_LL1181: _temp1180=(( struct Cyc_Absyn_TupleType_struct*) _temp1174)->f1; goto
_LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1180) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc, _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u)); goto _LL1175;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1180=(( struct Cyc_List_List*) _check_null(
_temp1180))->tl)){ void* _temp1182=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1180))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1182,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1183=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1183->hd=( void*)({ struct _tuple8*
_temp1184=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1184->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1180))->hd)).f1;
_temp1184->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1184;});
_temp1183->tl= fields; _temp1183;});} done= 1; goto _LL1175; _LL1179: goto
_LL1175; _LL1175:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1185->hd=( void*)({ struct _tuple8* _temp1186=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1186->f1= Cyc_Absyn_empty_tqual(); _temp1186->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1186;}); _temp1185->tl=
fields; _temp1185;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1187=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1187[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1188; _temp1188.tag= Cyc_Absyn_TupleType;
_temp1188.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1188;}); _temp1187;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1208=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1208->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1208;})); struct Cyc_Absyn_Const_e_struct*
_temp1189=({ struct Cyc_Absyn_Const_e_struct* _temp1204=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1204[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1205; _temp1205.tag= Cyc_Absyn_Const_e; _temp1205.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1206=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1206[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1207; _temp1207.tag= Cyc_Absyn_Int_c; _temp1207.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1207.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1207;}); _temp1206;})); _temp1205;}); _temp1204;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1189, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1190=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1190->v=( void*) Cyc_Absyn_uint_t; _temp1190;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1202=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1202[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1203; _temp1203.tag= Cyc_Absyn_ArrayType;
_temp1203.f1=( void*) res; _temp1203.f2= Cyc_Absyn_empty_tqual(); _temp1203.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1203;}); _temp1202;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1191= Cyc_Absynpp_typ2string( res);
xprintf("elements of array do not all have the same type (%.*s)", _get_arr_size(
_temp1191, 1u), _temp1191.curr);}));}{ int offset= 0; for( 0; des != 0;( offset
++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1;
if( ds != 0){ void* _temp1192=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tagged_arr* _temp1198; struct Cyc_Absyn_Exp* _temp1200; _LL1194: if(*((
int*) _temp1192) == Cyc_Absyn_FieldName){ _LL1199: _temp1198=(( struct Cyc_Absyn_FieldName_struct*)
_temp1192)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(*(( int*)
_temp1192) == Cyc_Absyn_ArrayElement){ _LL1201: _temp1200=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1192)->f1; goto _LL1197;} else{ goto _LL1193;} _LL1195: Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u)); goto _LL1193; _LL1197: Cyc_Tcexp_tcExpInitializer( te, 0,
_temp1200);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1200); if( i !=
offset){ Cyc_Tcutil_terr( _temp1200->loc,( struct _tagged_arr) xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1193;} _LL1193:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1209= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_get_arr_size( _temp1209, 1u), _temp1209.curr);}));} if( !( vd->tq).q_const){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1210= 0; struct Cyc_Absyn_Tqual*
_temp1211= 0; if( topt != 0){ void* _temp1212= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1220; struct Cyc_Absyn_Exp*
_temp1222; struct Cyc_Absyn_Tqual _temp1224; struct Cyc_Absyn_Tqual* _temp1226;
void* _temp1227; void** _temp1229; _LL1214: if(( unsigned int) _temp1212 > 4u?*((
int*) _temp1212) == Cyc_Absyn_PointerType: 0){ _LL1221: _temp1220=(( struct Cyc_Absyn_PointerType_struct*)
_temp1212)->f1; goto _LL1215;} else{ goto _LL1216;} _LL1216: if(( unsigned int)
_temp1212 > 4u?*(( int*) _temp1212) == Cyc_Absyn_ArrayType: 0){ _LL1228:
_temp1227=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f1;
_temp1229=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f1); goto
_LL1225; _LL1225: _temp1224=(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f2;
_temp1226=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f2; goto _LL1223;
_LL1223: _temp1222=(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f3; goto
_LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219; _LL1215: pinfo= _temp1220;
_temp1210=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1211=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1213; _LL1217: _temp1210=( void**) _temp1229; _temp1211=(
struct Cyc_Absyn_Tqual*) _temp1226; goto _LL1213; _LL1219: goto _LL1213; _LL1213:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1210, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1230=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1230[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1231; _temp1231.tag= Cyc_Absyn_ArrayType;
_temp1231.f1=( void*) t; _temp1231.f2= _temp1211 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1211)); _temp1231.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1231;}); _temp1230;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1232; _push_handler(&
_temp1232);{ int _temp1234= 0; if( setjmp( _temp1232.handler)){ _temp1234= 1;}
if( ! _temp1234){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1233=( void*) _exn_thrown; void* _temp1236= _temp1233; _LL1238:
if( _temp1236 == Cyc_Dict_Absent){ goto _LL1239;} else{ goto _LL1240;} _LL1240:
goto _LL1241; _LL1239: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1242= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_get_arr_size( _temp1242, 1u), _temp1242.curr);})); return topt != 0?*(( void**)
_check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1241:( void) _throw(
_temp1236); _LL1237:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1243= _new_region(); struct _RegionHandle* rgn=&
_temp1243; _push_region( rgn);{ struct _tuple4 _temp1244=({ struct _tuple4
_temp1266; _temp1266.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1266.f2= rgn;
_temp1266;}); struct Cyc_List_List* _temp1245=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1244, sd->tvs); struct Cyc_List_List* _temp1246=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1245); struct Cyc_Absyn_StructType_struct*
_temp1247=({ struct Cyc_Absyn_StructType_struct* _temp1263=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1263[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1264; _temp1264.tag= Cyc_Absyn_StructType;
_temp1264.f1=( struct _tuple1*)* tn; _temp1264.f2= _temp1246; _temp1264.f3=({
struct Cyc_Absyn_Structdecl** _temp1265=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1265[ 0]= sd; _temp1265;});
_temp1264;}); _temp1263;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1247,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1248=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1248->v=( void*)
_temp1246; _temp1248;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc, _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u));{ void* _temp1249=( void*) _temp1247;
_npop_handler( 0u); return _temp1249;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1252; struct Cyc_Absyn_Exp* _temp1253; struct Cyc_Absyn_Structfield*
_temp1255; struct _tuple9* _temp1250=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1252=* _temp1250; _LL1256: _temp1255= _temp1252.f1;
goto _LL1254; _LL1254: _temp1253= _temp1252.f2; goto _LL1251; _LL1251: { void*
_temp1257= Cyc_Tcutil_rsubstitute( rgn, _temp1245,( void*) _temp1255->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1257, _temp1253); if( ! Cyc_Tcutil_coerce_arg( te, _temp1253,
_temp1257)){ Cyc_Tcutil_terr( _temp1253->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1258=* _temp1255->name; struct _tagged_arr _temp1259= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_arr _temp1260= Cyc_Absynpp_typ2string( _temp1257); struct
_tagged_arr _temp1261= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1253->topt))->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_get_arr_size( _temp1258, 1u), _temp1258.curr, _get_arr_size( _temp1259, 1u),
_temp1259.curr, _get_arr_size( _temp1260, 1u), _temp1260.curr, _get_arr_size(
_temp1261, 1u), _temp1261.curr);}));}}}{ void* _temp1262=( void*) _temp1247;
_npop_handler( 0u); return _temp1262;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1267= _new_region(); struct
_RegionHandle* rgn=& _temp1267; _push_region( rgn);{ void* _temp1268= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1274; _LL1270: if(( unsigned int) _temp1268 > 4u?*((
int*) _temp1268) == Cyc_Absyn_AnonStructType: 0){ _LL1275: _temp1274=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1268)->f1; goto _LL1271;} else{ goto
_LL1272;} _LL1272: goto _LL1273; _LL1271: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1274); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1278; struct Cyc_Absyn_Exp*
_temp1279; struct Cyc_Absyn_Structfield* _temp1281; struct _tuple9* _temp1276=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1278=*
_temp1276; _LL1282: _temp1281= _temp1278.f1; goto _LL1280; _LL1280: _temp1279=
_temp1278.f2; goto _LL1277; _LL1277: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1281->type)), _temp1279); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1279,( void*) _temp1281->type)){ Cyc_Tcutil_terr( _temp1279->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1283=* _temp1281->name; struct
_tagged_arr _temp1284= Cyc_Absynpp_typ2string(( void*) _temp1281->type); struct
_tagged_arr _temp1285= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1279->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_get_arr_size( _temp1283, 1u), _temp1283.curr, _get_arr_size( _temp1284, 1u),
_temp1284.curr, _get_arr_size( _temp1285, 1u), _temp1285.curr);}));}} goto
_LL1269;} _LL1273:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1269; _LL1269:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1286=
_new_region(); struct _RegionHandle* rgn=& _temp1286; _push_region( rgn);{
struct _tuple4 _temp1287=({ struct _tuple4 _temp1333; _temp1333.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1333.f2= rgn; _temp1333;}); struct Cyc_List_List* _temp1288=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1287, tud->tvs); struct Cyc_List_List*
_temp1289=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1287, tuf->tvs);
struct Cyc_List_List* _temp1290=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1288); struct Cyc_List_List* _temp1291=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1289); struct Cyc_List_List* _temp1292=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1288, _temp1289);* all_ref=({
struct Cyc_Core_Opt* _temp1293=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1293->v=( void*) _temp1290; _temp1293;});* exist_ref=({
struct Cyc_Core_Opt* _temp1294=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1294->v=( void*) _temp1291; _temp1294;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1328=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1328[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1329; _temp1329.tag= Cyc_Absyn_TunionFieldType;
_temp1329.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1330; _temp1330.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1331=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1331[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1332; _temp1332.tag=
Cyc_Absyn_KnownTunionfield; _temp1332.f1= tud; _temp1332.f2= tuf; _temp1332;});
_temp1331;})); _temp1330.targs= _temp1290; _temp1330;}); _temp1329;}); _temp1328;});
if( topt != 0){ void* _temp1295= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionFieldInfo _temp1303; struct Cyc_Absyn_TunionInfo
_temp1305; void* _temp1307; struct Cyc_List_List* _temp1309; void* _temp1311;
_LL1297: if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_TunionFieldType:
0){ _LL1304: _temp1303=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1295)->f1;
goto _LL1298;} else{ goto _LL1299;} _LL1299: if(( unsigned int) _temp1295 > 4u?*((
int*) _temp1295) == Cyc_Absyn_TunionType: 0){ _LL1306: _temp1305=(( struct Cyc_Absyn_TunionType_struct*)
_temp1295)->f1; _LL1312: _temp1311=( void*) _temp1305.tunion_info; goto _LL1310;
_LL1310: _temp1309= _temp1305.targs; goto _LL1308; _LL1308: _temp1307=( void*)
_temp1305.rgn; goto _LL1300;} else{ goto _LL1301;} _LL1301: goto _LL1302;
_LL1298: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1296;
_LL1300:{ struct Cyc_List_List* a= _temp1290; for( 0; a != 0? _temp1309 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1309=(( struct Cyc_List_List*)
_check_null( _temp1309))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1309))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1313=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1313->v=(
void*) res; _temp1313;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1314=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1314[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1315; _temp1315.tag=
Cyc_Absyn_TunionType; _temp1315.f1=({ struct Cyc_Absyn_TunionInfo _temp1316;
_temp1316.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1317=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1317[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1318; _temp1318.tag=
Cyc_Absyn_KnownTunion; _temp1318.f1= tud; _temp1318;}); _temp1317;})); _temp1316.targs=
_temp1290; _temp1316.rgn=( void*) _temp1307; _temp1316;}); _temp1315;});
_temp1314;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1319= res;
_npop_handler( 0u); return _temp1319;}} goto _LL1296; _LL1302: goto _LL1296;
_LL1296:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1292,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1320= Cyc_Absynpp_qvar2string( tuf->name); struct _tagged_arr _temp1321=
Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp1322= e->topt == 0? _tag_arr("?",
sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_get_arr_size( _temp1320, 1u), _temp1320.curr, _get_arr_size( _temp1321, 1u),
_temp1321.curr, _get_arr_size( _temp1322, 1u), _temp1322.curr);}));}} if( es !=
0){ void* _temp1324= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1323= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp1323, 1u), _temp1323.curr);})); _npop_handler( 0u); return
_temp1324;} if( ts != 0){ void* _temp1326= Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1325= Cyc_Absynpp_qvar2string( tuf->name);
xprintf("too few arguments for tunion constructor %.*s", _get_arr_size(
_temp1325, 1u), _temp1325.curr);})); _npop_handler( 0u); return _temp1326;}{
void* _temp1327= res; _npop_handler( 0u); return _temp1327;}}}}; _pop_region(
rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1343=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1343[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1344; _temp1344.tag= Cyc_Absyn_RgnHandleType; _temp1344.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1345=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1345->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1345;})); _temp1344;}); _temp1343;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1334= Cyc_Tcutil_compress( handle_type); void*
_temp1340; _LL1336: if(( unsigned int) _temp1334 > 4u?*(( int*) _temp1334) ==
Cyc_Absyn_RgnHandleType: 0){ _LL1341: _temp1340=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1334)->f1; goto _LL1337;} else{ goto _LL1338;} _LL1338: goto _LL1339;
_LL1337: rgn= _temp1340; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1335; _LL1339: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null( ropt))->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1342= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1342, 1u), _temp1342.curr);})); goto _LL1335; _LL1335:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1346= t; struct Cyc_Absyn_TunionFieldInfo _temp1352; struct Cyc_List_List*
_temp1354; void* _temp1356; struct Cyc_Absyn_Tunionfield* _temp1358; struct Cyc_Absyn_Tuniondecl*
_temp1360; _LL1348: if(( unsigned int) _temp1346 > 4u?*(( int*) _temp1346) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1353: _temp1352=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1346)->f1; _LL1357: _temp1356=( void*) _temp1352.field_info; if(*(( int*)
_temp1356) == Cyc_Absyn_KnownTunionfield){ _LL1361: _temp1360=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1356)->f1; goto _LL1359; _LL1359: _temp1358=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1356)->f2; goto _LL1355;} else{ goto _LL1350;} _LL1355: _temp1354=
_temp1352.targs; goto _LL1349;} else{ goto _LL1350;} _LL1350: goto _LL1351;
_LL1349: if( _temp1358->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1347; _LL1351: goto _LL1347; _LL1347:;}{
void*(* _temp1362)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1363= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1369; struct Cyc_Absyn_Conref* _temp1371;
struct Cyc_Absyn_Tqual _temp1373; struct Cyc_Absyn_Conref* _temp1375; void*
_temp1377; void* _temp1379; _LL1365: if(( unsigned int) _temp1363 > 4u?*(( int*)
_temp1363) == Cyc_Absyn_PointerType: 0){ _LL1370: _temp1369=(( struct Cyc_Absyn_PointerType_struct*)
_temp1363)->f1; _LL1380: _temp1379=( void*) _temp1369.elt_typ; goto _LL1378;
_LL1378: _temp1377=( void*) _temp1369.rgn_typ; goto _LL1376; _LL1376: _temp1375=
_temp1369.nullable; goto _LL1374; _LL1374: _temp1373= _temp1369.tq; goto _LL1372;
_LL1372: _temp1371= _temp1369.bounds; goto _LL1366;} else{ goto _LL1367;}
_LL1367: goto _LL1368; _LL1366: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1375)){ _temp1362= Cyc_Absyn_star_typ;} goto _LL1364; _LL1368: goto _LL1364;
_LL1364:;} return _temp1362( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1381=( void*) s->r; struct Cyc_Absyn_Exp* _temp1391;
struct Cyc_Absyn_Stmt* _temp1393; struct Cyc_Absyn_Stmt* _temp1395; struct Cyc_Absyn_Stmt*
_temp1397; struct Cyc_Absyn_Decl* _temp1399; _LL1383: if(( unsigned int)
_temp1381 > 1u?*(( int*) _temp1381) == Cyc_Absyn_Exp_s: 0){ _LL1392: _temp1391=((
struct Cyc_Absyn_Exp_s_struct*) _temp1381)->f1; goto _LL1384;} else{ goto
_LL1385;} _LL1385: if(( unsigned int) _temp1381 > 1u?*(( int*) _temp1381) == Cyc_Absyn_Seq_s:
0){ _LL1396: _temp1395=(( struct Cyc_Absyn_Seq_s_struct*) _temp1381)->f1; goto
_LL1394; _LL1394: _temp1393=(( struct Cyc_Absyn_Seq_s_struct*) _temp1381)->f2;
goto _LL1386;} else{ goto _LL1387;} _LL1387: if(( unsigned int) _temp1381 > 1u?*((
int*) _temp1381) == Cyc_Absyn_Decl_s: 0){ _LL1400: _temp1399=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1381)->f1; goto _LL1398; _LL1398: _temp1397=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1381)->f2; goto _LL1388;} else{ goto _LL1389;} _LL1389: goto _LL1390;
_LL1384: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1391->topt))->v;
_LL1386: s= _temp1393; continue; _LL1388: s= _temp1397; continue; _LL1390:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1382:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1410=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1410[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1411; _temp1411.tag= Cyc_Absyn_RgnHandleType; _temp1411.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1412=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1412->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1412;})); _temp1411;}); _temp1410;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1401= Cyc_Tcutil_compress( handle_type);
void* _temp1407; _LL1403: if(( unsigned int) _temp1401 > 4u?*(( int*) _temp1401)
== Cyc_Absyn_RgnHandleType: 0){ _LL1408: _temp1407=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1401)->f1; goto _LL1404;} else{ goto _LL1405;} _LL1405: goto _LL1406;
_LL1404: rgn= _temp1407; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1402; _LL1406: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1409= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1409, 1u), _temp1409.curr);})); goto _LL1402; _LL1402:;}{ void* _temp1413=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1425; struct Cyc_Absyn_Exp* _temp1427;
struct Cyc_Absyn_Vardecl* _temp1429; struct Cyc_List_List* _temp1431; struct Cyc_Core_Opt*
_temp1433; struct Cyc_List_List* _temp1435; void* _temp1437; struct _tagged_arr
_temp1439; _LL1415: if(*(( int*) _temp1413) == Cyc_Absyn_Comprehension_e){
_LL1430: _temp1429=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1413)->f1;
goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1413)->f2; goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1413)->f3; goto _LL1416;} else{ goto _LL1417;} _LL1417: if(*(( int*)
_temp1413) == Cyc_Absyn_UnresolvedMem_e){ _LL1434: _temp1433=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1413)->f1; goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1413)->f2; goto _LL1418;} else{ goto _LL1419;} _LL1419: if(*(( int*)
_temp1413) == Cyc_Absyn_Array_e){ _LL1436: _temp1435=(( struct Cyc_Absyn_Array_e_struct*)
_temp1413)->f1; goto _LL1420;} else{ goto _LL1421;} _LL1421: if(*(( int*)
_temp1413) == Cyc_Absyn_Const_e){ _LL1438: _temp1437=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1413)->f1; if(( unsigned int) _temp1437 > 1u?*(( int*) _temp1437) == Cyc_Absyn_String_c:
0){ _LL1440: _temp1439=(( struct Cyc_Absyn_String_c_struct*) _temp1437)->f1;
goto _LL1422;} else{ goto _LL1423;}} else{ goto _LL1423;} _LL1423: goto _LL1424;
_LL1416: { void* _temp1441= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1442= Cyc_Tcutil_compress( _temp1441); struct Cyc_Absyn_Exp* _temp1448;
struct Cyc_Absyn_Tqual _temp1450; void* _temp1452; _LL1444: if(( unsigned int)
_temp1442 > 4u?*(( int*) _temp1442) == Cyc_Absyn_ArrayType: 0){ _LL1453:
_temp1452=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1442)->f1; goto
_LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_ArrayType_struct*) _temp1442)->f2;
goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1442)->f3; goto _LL1445;} else{ goto _LL1446;} _LL1446: goto _LL1447;
_LL1445: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1427)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1458=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1458[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1459; _temp1459.tag= Cyc_Absyn_Upper_b; _temp1459.f1=
_temp1427; _temp1459;}); _temp1458;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1455=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1455[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1456; _temp1456.tag= Cyc_Absyn_PointerType;
_temp1456.f1=({ struct Cyc_Absyn_PtrInfo _temp1457; _temp1457.elt_typ=( void*)
_temp1452; _temp1457.rgn_typ=( void*) rgn; _temp1457.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1457.tq= _temp1450; _temp1457.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1457;});
_temp1456;}); _temp1455;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1454=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1454->v=(
void*) res_typ; _temp1454;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1447: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1443:;} _LL1418:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1460=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1460[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1461; _temp1461.tag= Cyc_Absyn_Array_e; _temp1461.f1=
_temp1431; _temp1461;}); _temp1460;}))); _temp1435= _temp1431; goto _LL1420;
_LL1420: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1462= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1468; struct Cyc_Absyn_Conref*
_temp1470; struct Cyc_Absyn_Tqual _temp1472; struct Cyc_Absyn_Conref* _temp1474;
void* _temp1476; void* _temp1478; void** _temp1480; _LL1464: if(( unsigned int)
_temp1462 > 4u?*(( int*) _temp1462) == Cyc_Absyn_PointerType: 0){ _LL1469:
_temp1468=(( struct Cyc_Absyn_PointerType_struct*) _temp1462)->f1; _LL1479:
_temp1478=( void*) _temp1468.elt_typ; _temp1480=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1462)->f1).elt_typ; goto _LL1477; _LL1477: _temp1476=( void*) _temp1468.rgn_typ;
goto _LL1475; _LL1475: _temp1474= _temp1468.nullable; goto _LL1473; _LL1473:
_temp1472= _temp1468.tq; goto _LL1471; _LL1471: _temp1470= _temp1468.bounds;
goto _LL1465;} else{ goto _LL1466;} _LL1466: goto _LL1467; _LL1465: elt_typ_opt=(
void**) _temp1480; goto _LL1463; _LL1467: goto _LL1463; _LL1463:;}{ void*
_temp1481= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1482= Cyc_Tcutil_compress( _temp1481); struct Cyc_Absyn_Exp* _temp1488;
struct Cyc_Absyn_Tqual _temp1490; void* _temp1492; _LL1484: if(( unsigned int)
_temp1482 > 4u?*(( int*) _temp1482) == Cyc_Absyn_ArrayType: 0){ _LL1493:
_temp1492=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1482)->f1; goto
_LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_ArrayType_struct*) _temp1482)->f2;
goto _LL1489; _LL1489: _temp1488=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1482)->f3; goto _LL1485;} else{ goto _LL1486;} _LL1486: goto _LL1487;
_LL1485: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1494=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1494[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1495; _temp1495.tag=
Cyc_Absyn_PointerType; _temp1495.f1=({ struct Cyc_Absyn_PtrInfo _temp1496;
_temp1496.elt_typ=( void*) _temp1492; _temp1496.rgn_typ=( void*) rgn; _temp1496.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1496.tq=
_temp1490; _temp1496.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1497=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1497[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1498; _temp1498.tag= Cyc_Absyn_Upper_b; _temp1498.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1488); _temp1498;}); _temp1497;}));
_temp1496;}); _temp1495;}); _temp1494;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1499=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1499->v=( void*) res_typ; _temp1499;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1483; _LL1487: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcExpNoPromote on Array_e returned non-array type", sizeof(
unsigned char), 50u)); _LL1483:;} return res_typ;}} _LL1422: { void* _temp1500=
Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
void* _temp1501= Cyc_Tcexp_tcExp( te,( void**)& _temp1500, e1); return Cyc_Absyn_atb_typ(
_temp1501, rgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1502=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1502[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1503; _temp1503.tag= Cyc_Absyn_Upper_b;
_temp1503.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1503;}); _temp1502;}));} _LL1424: {
void** topt2= 0; if( topt != 0){ void* _temp1504= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1512; struct Cyc_Absyn_Conref*
_temp1514; struct Cyc_Absyn_Tqual _temp1516; struct Cyc_Absyn_Conref* _temp1518;
void* _temp1520; void* _temp1522; void** _temp1524; struct Cyc_Absyn_TunionInfo
_temp1525; _LL1506: if(( unsigned int) _temp1504 > 4u?*(( int*) _temp1504) ==
Cyc_Absyn_PointerType: 0){ _LL1513: _temp1512=(( struct Cyc_Absyn_PointerType_struct*)
_temp1504)->f1; _LL1523: _temp1522=( void*) _temp1512.elt_typ; _temp1524=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1504)->f1).elt_typ; goto _LL1521;
_LL1521: _temp1520=( void*) _temp1512.rgn_typ; goto _LL1519; _LL1519: _temp1518=
_temp1512.nullable; goto _LL1517; _LL1517: _temp1516= _temp1512.tq; goto _LL1515;
_LL1515: _temp1514= _temp1512.bounds; goto _LL1507;} else{ goto _LL1508;}
_LL1508: if(( unsigned int) _temp1504 > 4u?*(( int*) _temp1504) == Cyc_Absyn_TunionType:
0){ _LL1526: _temp1525=(( struct Cyc_Absyn_TunionType_struct*) _temp1504)->f1;
goto _LL1509;} else{ goto _LL1510;} _LL1510: goto _LL1511; _LL1507: topt2=( void**)
_temp1524; goto _LL1505; _LL1509: topt2=({ void** _temp1527=( void**) GC_malloc(
sizeof( void*)); _temp1527[ 0]=*(( void**) _check_null( topt)); _temp1527;});
goto _LL1505; _LL1511: goto _LL1505; _LL1505:;}{ void* _temp1528= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1530=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1530[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1531; _temp1531.tag=
Cyc_Absyn_PointerType; _temp1531.f1=({ struct Cyc_Absyn_PtrInfo _temp1532;
_temp1532.elt_typ=( void*) _temp1528; _temp1532.rgn_typ=( void*) rgn; _temp1532.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1532.tq= Cyc_Absyn_empty_tqual();
_temp1532.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1533=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1533[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1534; _temp1534.tag= Cyc_Absyn_Upper_b; _temp1534.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1534;}); _temp1533;})); _temp1532;}); _temp1531;});
_temp1530;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1529=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1529->v=( void*) res_typ;
_temp1529;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}} _LL1414:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1535= t; struct Cyc_Absyn_Exp* _temp1541; struct Cyc_Absyn_Tqual
_temp1543; void* _temp1545; _LL1537: if(( unsigned int) _temp1535 > 4u?*(( int*)
_temp1535) == Cyc_Absyn_ArrayType: 0){ _LL1546: _temp1545=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f1; goto _LL1544; _LL1544: _temp1543=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f2; goto _LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f3; goto _LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540;
_LL1538: { void* _temp1549; int _temp1551; struct _tuple6 _temp1547= Cyc_Tcutil_addressof_props(
te, e); _LL1552: _temp1551= _temp1547.f1; goto _LL1550; _LL1550: _temp1549=
_temp1547.f2; goto _LL1548; _LL1548: { void* _temp1553= _temp1541 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1554=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1554[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1555; _temp1555.tag= Cyc_Absyn_Upper_b;
_temp1555.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1541); _temp1555;});
_temp1554;}); t= Cyc_Absyn_atb_typ( _temp1545, _temp1549, _temp1543, _temp1553);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1540: return t; _LL1536:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1556=( void*) e->r; struct Cyc_List_List* _temp1566; struct
Cyc_Absyn_Exp* _temp1568; struct Cyc_Absyn_Exp* _temp1570; struct Cyc_Absyn_Vardecl*
_temp1572; void* _temp1574; struct _tagged_arr _temp1576; _LL1558: if(*(( int*)
_temp1556) == Cyc_Absyn_Array_e){ _LL1567: _temp1566=(( struct Cyc_Absyn_Array_e_struct*)
_temp1556)->f1; goto _LL1559;} else{ goto _LL1560;} _LL1560: if(*(( int*)
_temp1556) == Cyc_Absyn_Comprehension_e){ _LL1573: _temp1572=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1556)->f1; goto _LL1571; _LL1571: _temp1570=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1556)->f2; goto _LL1569; _LL1569: _temp1568=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1556)->f3; goto _LL1561;} else{ goto _LL1562;} _LL1562: if(*(( int*)
_temp1556) == Cyc_Absyn_Const_e){ _LL1575: _temp1574=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1556)->f1; if(( unsigned int) _temp1574 > 1u?*(( int*) _temp1574) == Cyc_Absyn_String_c:
0){ _LL1577: _temp1576=(( struct Cyc_Absyn_String_c_struct*) _temp1574)->f1;
goto _LL1563;} else{ goto _LL1564;}} else{ goto _LL1564;} _LL1564: goto _LL1565;
_LL1559: return t; _LL1561: return t; _LL1563: return t; _LL1565: t= Cyc_Tcutil_compress(
t);{ void* _temp1578= t; struct Cyc_Absyn_Exp* _temp1584; struct Cyc_Absyn_Tqual
_temp1586; void* _temp1588; _LL1580: if(( unsigned int) _temp1578 > 4u?*(( int*)
_temp1578) == Cyc_Absyn_ArrayType: 0){ _LL1589: _temp1588=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1578)->f1; goto _LL1587; _LL1587: _temp1586=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1578)->f2; goto _LL1585; _LL1585: _temp1584=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1578)->f3; goto _LL1581;} else{ goto _LL1582;} _LL1582: goto _LL1583;
_LL1581: { void* _temp1592; int _temp1594; struct _tuple6 _temp1590= Cyc_Tcutil_addressof_props(
te, e); _LL1595: _temp1594= _temp1590.f1; goto _LL1593; _LL1593: _temp1592=
_temp1590.f2; goto _LL1591; _LL1591: { void* b= _temp1584 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1596=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1596[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1597; _temp1597.tag= Cyc_Absyn_Upper_b; _temp1597.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1584); _temp1597;}); _temp1596;}); t=
Cyc_Absyn_atb_typ( _temp1588, _temp1592, _temp1586, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1583: return t; _LL1579:;} _LL1557:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1598=( void*) e->r; struct Cyc_Absyn_Exp* _temp1604; _LL1600: if(*(( int*)
_temp1598) == Cyc_Absyn_NoInstantiate_e){ _LL1605: _temp1604=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1598)->f1; goto _LL1601;} else{ goto _LL1602;} _LL1602: goto _LL1603;
_LL1601: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1604);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1604->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1604->topt))->v))); e->topt=
_temp1604->topt; goto _LL1599; _LL1603: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1606= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1612; struct Cyc_Absyn_Conref*
_temp1614; struct Cyc_Absyn_Tqual _temp1616; struct Cyc_Absyn_Conref* _temp1618;
void* _temp1620; void* _temp1622; _LL1608: if(( unsigned int) _temp1606 > 4u?*((
int*) _temp1606) == Cyc_Absyn_PointerType: 0){ _LL1613: _temp1612=(( struct Cyc_Absyn_PointerType_struct*)
_temp1606)->f1; _LL1623: _temp1622=( void*) _temp1612.elt_typ; goto _LL1621;
_LL1621: _temp1620=( void*) _temp1612.rgn_typ; goto _LL1619; _LL1619: _temp1618=
_temp1612.nullable; goto _LL1617; _LL1617: _temp1616= _temp1612.tq; goto _LL1615;
_LL1615: _temp1614= _temp1612.bounds; goto _LL1609;} else{ goto _LL1610;}
_LL1610: goto _LL1611; _LL1609:{ void* _temp1624= Cyc_Tcutil_compress( _temp1622);
struct Cyc_Absyn_FnInfo _temp1630; struct Cyc_List_List* _temp1632; struct Cyc_List_List*
_temp1634; struct Cyc_Absyn_VarargInfo* _temp1636; int _temp1638; struct Cyc_List_List*
_temp1640; void* _temp1642; struct Cyc_Core_Opt* _temp1644; struct Cyc_List_List*
_temp1646; _LL1626: if(( unsigned int) _temp1624 > 4u?*(( int*) _temp1624) ==
Cyc_Absyn_FnType: 0){ _LL1631: _temp1630=(( struct Cyc_Absyn_FnType_struct*)
_temp1624)->f1; _LL1647: _temp1646= _temp1630.tvars; goto _LL1645; _LL1645:
_temp1644= _temp1630.effect; goto _LL1643; _LL1643: _temp1642=( void*) _temp1630.ret_typ;
goto _LL1641; _LL1641: _temp1640= _temp1630.args; goto _LL1639; _LL1639:
_temp1638= _temp1630.c_varargs; goto _LL1637; _LL1637: _temp1636= _temp1630.cyc_varargs;
goto _LL1635; _LL1635: _temp1634= _temp1630.rgn_po; goto _LL1633; _LL1633:
_temp1632= _temp1630.attributes; goto _LL1627;} else{ goto _LL1628;} _LL1628:
goto _LL1629; _LL1627: if( _temp1646 != 0){ struct _RegionHandle _temp1648=
_new_region(); struct _RegionHandle* rgn=& _temp1648; _push_region( rgn);{
struct _tuple4 _temp1649=({ struct _tuple4 _temp1659; _temp1659.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1659.f2= rgn; _temp1659;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1649, _temp1646); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1656=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1656[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1657; _temp1657.tag= Cyc_Absyn_FnType; _temp1657.f1=({ struct Cyc_Absyn_FnInfo
_temp1658; _temp1658.tvars= 0; _temp1658.effect= _temp1644; _temp1658.ret_typ=(
void*) _temp1642; _temp1658.args= _temp1640; _temp1658.c_varargs= _temp1638;
_temp1658.cyc_varargs= _temp1636; _temp1658.rgn_po= _temp1634; _temp1658.attributes=
_temp1632; _temp1658;}); _temp1657;}); _temp1656;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1653=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1653[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1654; _temp1654.tag= Cyc_Absyn_PointerType;
_temp1654.f1=({ struct Cyc_Absyn_PtrInfo _temp1655; _temp1655.elt_typ=( void*)
ftyp; _temp1655.rgn_typ=( void*) _temp1620; _temp1655.nullable= _temp1618;
_temp1655.tq= _temp1616; _temp1655.bounds= _temp1614; _temp1655;}); _temp1654;});
_temp1653;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1650=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1650[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1651; _temp1651.tag= Cyc_Absyn_Instantiate_e;
_temp1651.f1= inner; _temp1651.f2= ts; _temp1651;}); _temp1650;}))); e->topt=({
struct Cyc_Core_Opt* _temp1652=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1652->v=( void*) new_typ; _temp1652;});}; _pop_region( rgn);}
goto _LL1625; _LL1629: goto _LL1625; _LL1625:;} goto _LL1607; _LL1611: goto
_LL1607; _LL1607:;} goto _LL1599; _LL1599:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1660=( void*) e->r; struct Cyc_Absyn_Exp* _temp1734; struct
_tuple1* _temp1736; struct Cyc_List_List* _temp1738; struct Cyc_Absyn_Exp*
_temp1740; struct Cyc_List_List* _temp1742; struct Cyc_Core_Opt* _temp1744; void*
_temp1746; void* _temp1748; struct _tuple1* _temp1750; struct Cyc_List_List*
_temp1752; void* _temp1754; void* _temp1756; struct Cyc_Absyn_Exp* _temp1758;
struct Cyc_Absyn_Exp* _temp1760; struct Cyc_Core_Opt* _temp1762; struct Cyc_Absyn_Exp*
_temp1764; struct Cyc_Absyn_Exp* _temp1766; struct Cyc_Absyn_Exp* _temp1768;
struct Cyc_Absyn_Exp* _temp1770; struct Cyc_Absyn_Exp* _temp1772; struct Cyc_Absyn_Exp*
_temp1774; struct Cyc_Absyn_VarargCallInfo* _temp1776; struct Cyc_Absyn_VarargCallInfo**
_temp1778; struct Cyc_List_List* _temp1779; struct Cyc_Absyn_Exp* _temp1781;
struct Cyc_Absyn_Exp* _temp1783; struct Cyc_List_List* _temp1785; struct Cyc_Absyn_Exp*
_temp1787; struct Cyc_Absyn_Exp* _temp1789; void* _temp1791; struct Cyc_Absyn_Exp*
_temp1793; struct Cyc_Absyn_Exp* _temp1795; struct Cyc_Absyn_Exp* _temp1797;
struct Cyc_Absyn_Exp* _temp1799; void* _temp1801; struct _tagged_arr* _temp1803;
void* _temp1805; struct Cyc_Absyn_Exp* _temp1807; struct _tagged_arr* _temp1809;
struct Cyc_Absyn_Exp* _temp1811; struct _tagged_arr* _temp1813; struct Cyc_Absyn_Exp*
_temp1815; struct Cyc_Absyn_Exp* _temp1817; struct Cyc_Absyn_Exp* _temp1819;
struct Cyc_List_List* _temp1821; struct Cyc_List_List* _temp1823; struct _tuple2*
_temp1825; struct Cyc_List_List* _temp1827; struct Cyc_Absyn_Stmt* _temp1829;
struct Cyc_Absyn_Fndecl* _temp1831; struct Cyc_Absyn_Exp* _temp1833; struct Cyc_Absyn_Exp*
_temp1835; struct Cyc_Absyn_Exp* _temp1837; struct Cyc_Absyn_Vardecl* _temp1839;
struct Cyc_Absyn_Structdecl* _temp1841; struct Cyc_Absyn_Structdecl** _temp1843;
struct Cyc_List_List* _temp1844; struct Cyc_Core_Opt* _temp1846; struct Cyc_Core_Opt**
_temp1848; struct _tuple1* _temp1849; struct _tuple1** _temp1851; struct Cyc_List_List*
_temp1852; void* _temp1854; struct Cyc_Absyn_Tunionfield* _temp1856; struct Cyc_Absyn_Tuniondecl*
_temp1858; struct Cyc_List_List* _temp1860; struct Cyc_Core_Opt* _temp1862;
struct Cyc_Core_Opt** _temp1864; struct Cyc_Core_Opt* _temp1865; struct Cyc_Core_Opt**
_temp1867; struct Cyc_Absyn_Enumfield* _temp1868; struct Cyc_Absyn_Enumdecl*
_temp1870; struct _tuple1* _temp1872; struct _tuple1** _temp1874; void*
_temp1875; struct Cyc_Absyn_Exp* _temp1877; _LL1662: if(*(( int*) _temp1660) ==
Cyc_Absyn_NoInstantiate_e){ _LL1735: _temp1734=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1660)->f1; goto _LL1663;} else{ goto _LL1664;} _LL1664: if(*(( int*)
_temp1660) == Cyc_Absyn_UnknownId_e){ _LL1737: _temp1736=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1660)->f1; goto _LL1665;} else{ goto _LL1666;} _LL1666: if(*(( int*)
_temp1660) == Cyc_Absyn_UnknownCall_e){ _LL1741: _temp1740=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1660)->f1; goto _LL1739; _LL1739: _temp1738=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1660)->f2; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(*(( int*)
_temp1660) == Cyc_Absyn_UnresolvedMem_e){ _LL1745: _temp1744=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1660)->f1; goto _LL1743; _LL1743: _temp1742=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1660)->f2; goto _LL1669;} else{ goto _LL1670;} _LL1670: if(*(( int*)
_temp1660) == Cyc_Absyn_Const_e){ _LL1747: _temp1746=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1660)->f1; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(*(( int*)
_temp1660) == Cyc_Absyn_Var_e){ _LL1751: _temp1750=(( struct Cyc_Absyn_Var_e_struct*)
_temp1660)->f1; goto _LL1749; _LL1749: _temp1748=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1660)->f2; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(*(( int*)
_temp1660) == Cyc_Absyn_Primop_e){ _LL1755: _temp1754=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1660)->f1; goto _LL1753; _LL1753: _temp1752=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1660)->f2; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(*(( int*)
_temp1660) == Cyc_Absyn_Increment_e){ _LL1759: _temp1758=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1660)->f1; goto _LL1757; _LL1757: _temp1756=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1660)->f2; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(*(( int*)
_temp1660) == Cyc_Absyn_AssignOp_e){ _LL1765: _temp1764=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1660)->f1; goto _LL1763; _LL1763: _temp1762=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1660)->f2; goto _LL1761; _LL1761: _temp1760=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1660)->f3; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(*(( int*)
_temp1660) == Cyc_Absyn_Conditional_e){ _LL1771: _temp1770=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1660)->f1; goto _LL1769; _LL1769: _temp1768=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1660)->f2; goto _LL1767; _LL1767: _temp1766=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1660)->f3; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*)
_temp1660) == Cyc_Absyn_SeqExp_e){ _LL1775: _temp1774=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1660)->f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1660)->f2; goto _LL1683;} else{ goto _LL1684;} _LL1684: if(*(( int*)
_temp1660) == Cyc_Absyn_FnCall_e){ _LL1782: _temp1781=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1660)->f1; goto _LL1780; _LL1780: _temp1779=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1660)->f2; goto _LL1777; _LL1777: _temp1776=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1660)->f3; _temp1778=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1660)->f3;
goto _LL1685;} else{ goto _LL1686;} _LL1686: if(*(( int*) _temp1660) == Cyc_Absyn_Throw_e){
_LL1784: _temp1783=(( struct Cyc_Absyn_Throw_e_struct*) _temp1660)->f1; goto
_LL1687;} else{ goto _LL1688;} _LL1688: if(*(( int*) _temp1660) == Cyc_Absyn_Instantiate_e){
_LL1788: _temp1787=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1660)->f1;
goto _LL1786; _LL1786: _temp1785=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1660)->f2; goto _LL1689;} else{ goto _LL1690;} _LL1690: if(*(( int*)
_temp1660) == Cyc_Absyn_Cast_e){ _LL1792: _temp1791=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1660)->f1; goto _LL1790; _LL1790: _temp1789=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1660)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(*(( int*)
_temp1660) == Cyc_Absyn_Address_e){ _LL1794: _temp1793=(( struct Cyc_Absyn_Address_e_struct*)
_temp1660)->f1; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1660) == Cyc_Absyn_New_e){ _LL1798: _temp1797=(( struct Cyc_Absyn_New_e_struct*)
_temp1660)->f1; goto _LL1796; _LL1796: _temp1795=(( struct Cyc_Absyn_New_e_struct*)
_temp1660)->f2; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(*(( int*)
_temp1660) == Cyc_Absyn_Sizeofexp_e){ _LL1800: _temp1799=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1660)->f1; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(*(( int*)
_temp1660) == Cyc_Absyn_Sizeoftyp_e){ _LL1802: _temp1801=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1660)->f1; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(*(( int*)
_temp1660) == Cyc_Absyn_Offsetof_e){ _LL1806: _temp1805=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1660)->f1; goto _LL1804; _LL1804: _temp1803=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1660)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(*(( int*)
_temp1660) == Cyc_Absyn_Deref_e){ _LL1808: _temp1807=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1660)->f1; goto _LL1703;} else{ goto _LL1704;} _LL1704: if(*(( int*)
_temp1660) == Cyc_Absyn_StructMember_e){ _LL1812: _temp1811=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1660)->f1; goto _LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1660)->f2; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(*(( int*)
_temp1660) == Cyc_Absyn_StructArrow_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1660)->f1; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1660)->f2; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(*(( int*)
_temp1660) == Cyc_Absyn_Subscript_e){ _LL1820: _temp1819=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1660)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1660)->f2; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(*(( int*)
_temp1660) == Cyc_Absyn_Tuple_e){ _LL1822: _temp1821=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1660)->f1; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1660) == Cyc_Absyn_CompoundLit_e){ _LL1826: _temp1825=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1660)->f1; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1660)->f2; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(*(( int*)
_temp1660) == Cyc_Absyn_Array_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_Array_e_struct*)
_temp1660)->f1; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(*(( int*)
_temp1660) == Cyc_Absyn_StmtExp_e){ _LL1830: _temp1829=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1660)->f1; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(*(( int*)
_temp1660) == Cyc_Absyn_Codegen_e){ _LL1832: _temp1831=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1660)->f1; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(*(( int*)
_temp1660) == Cyc_Absyn_Fill_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1660)->f1; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(*(( int*)
_temp1660) == Cyc_Absyn_Comprehension_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1660)->f1; goto _LL1838; _LL1838: _temp1837=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1660)->f2; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1660)->f3; goto _LL1723;} else{ goto _LL1724;} _LL1724: if(*(( int*)
_temp1660) == Cyc_Absyn_Struct_e){ _LL1850: _temp1849=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1660)->f1; _temp1851=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f1;
goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f2;
_temp1848=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f2; goto _LL1845;
_LL1845: _temp1844=(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f3; goto
_LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f4;
_temp1843=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1660)->f4; goto _LL1725;}
else{ goto _LL1726;} _LL1726: if(*(( int*) _temp1660) == Cyc_Absyn_AnonStruct_e){
_LL1855: _temp1854=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1660)->f1;
goto _LL1853; _LL1853: _temp1852=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1660)->f2; goto _LL1727;} else{ goto _LL1728;} _LL1728: if(*(( int*)
_temp1660) == Cyc_Absyn_Tunion_e){ _LL1866: _temp1865=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1660)->f1; _temp1867=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f1;
goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f2;
_temp1864=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f2; goto _LL1861;
_LL1861: _temp1860=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f3; goto
_LL1859; _LL1859: _temp1858=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f4;
goto _LL1857; _LL1857: _temp1856=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1660)->f5;
goto _LL1729;} else{ goto _LL1730;} _LL1730: if(*(( int*) _temp1660) == Cyc_Absyn_Enum_e){
_LL1873: _temp1872=(( struct Cyc_Absyn_Enum_e_struct*) _temp1660)->f1; _temp1874=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1660)->f1; goto _LL1871; _LL1871:
_temp1870=(( struct Cyc_Absyn_Enum_e_struct*) _temp1660)->f2; goto _LL1869;
_LL1869: _temp1868=(( struct Cyc_Absyn_Enum_e_struct*) _temp1660)->f3; goto
_LL1731;} else{ goto _LL1732;} _LL1732: if(*(( int*) _temp1660) == Cyc_Absyn_Malloc_e){
_LL1878: _temp1877=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1660)->f1; goto
_LL1876; _LL1876: _temp1875=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1660)->f2; goto _LL1733;} else{ goto _LL1661;} _LL1663: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1734); return; _LL1665: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1736); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1667: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1740, _temp1738); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1669: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1742); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1671: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1746, e);
goto _LL1661; _LL1673: t= Cyc_Tcexp_tcVar( te, loc, _temp1750, _temp1748); goto
_LL1661; _LL1675: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1754, _temp1752);
goto _LL1661; _LL1677: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1758,
_temp1756); goto _LL1661; _LL1679: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1764, _temp1762, _temp1760); goto _LL1661; _LL1681: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1770, _temp1768, _temp1766); goto _LL1661; _LL1683: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1774, _temp1772); goto _LL1661; _LL1685: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1781, _temp1779, _temp1778); goto _LL1661; _LL1687: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1783); goto _LL1661; _LL1689: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1787, _temp1785); goto _LL1661; _LL1691: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1791, _temp1789); goto _LL1661; _LL1693: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1793); goto _LL1661; _LL1695: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1797, e, _temp1795); goto _LL1661; _LL1697: { void* _temp1879= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1799); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1879); goto
_LL1661;} _LL1699: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1801); goto
_LL1661; _LL1701: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1805, _temp1803);
goto _LL1661; _LL1703: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1807); goto
_LL1661; _LL1705: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1811,
_temp1809); goto _LL1661; _LL1707: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1815, _temp1813); goto _LL1661; _LL1709: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1819, _temp1817); goto _LL1661; _LL1711: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1821); goto _LL1661; _LL1713: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1825, _temp1823); goto _LL1661; _LL1715: { void** elt_topt= 0;
if( topt != 0){ void* _temp1880= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1886; struct Cyc_Absyn_Tqual _temp1888; void*
_temp1890; void** _temp1892; _LL1882: if(( unsigned int) _temp1880 > 4u?*(( int*)
_temp1880) == Cyc_Absyn_ArrayType: 0){ _LL1891: _temp1890=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1880)->f1; _temp1892=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1880)->f1); goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1880)->f2; goto _LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1880)->f3; goto _LL1883;} else{ goto _LL1884;} _LL1884: goto _LL1885;
_LL1883: elt_topt=( void**) _temp1892; goto _LL1881; _LL1885: goto _LL1881;
_LL1881:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1827); goto _LL1661;}
_LL1717: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1829); goto _LL1661;
_LL1719: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1831); goto _LL1661;
_LL1721: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1833); goto _LL1661; _LL1723:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1839, _temp1837, _temp1835);
goto _LL1661; _LL1725: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1851,
_temp1848, _temp1844, _temp1843); goto _LL1661; _LL1727: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1854, _temp1852); goto _LL1661; _LL1729: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1867, _temp1864, _temp1860, _temp1858, _temp1856); goto
_LL1661; _LL1731:* _temp1874=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1868))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1893=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1893[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1894; _temp1894.tag= Cyc_Absyn_EnumType;
_temp1894.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1870))->name;
_temp1894.f2= _temp1870; _temp1894;}); _temp1893;}); goto _LL1661; _LL1733: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1877, _temp1875); goto _LL1661; _LL1661:;}
e->topt=({ struct Cyc_Core_Opt* _temp1895=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1895->v=( void*) t; _temp1895;});}